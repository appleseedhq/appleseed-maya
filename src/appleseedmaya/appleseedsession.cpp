
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016 Esteban Tovagliari, The appleseedhq Organization
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

// Interface header.
#include "appleseedmaya/appleseedsession.h"

// Standard headers.
#include <vector>

// Boost headers.
#include "boost/array.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/convenience.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/thread/thread.hpp"

// Maya headers.
#include <maya/MAnimControl.h>
#include <maya/MComputation.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MSelectionList.h>
#include <maya/MObject.h>
#include <maya/MObjectArray.h>

// appleseed.foundation headers.
#include "foundation/platform/timers.h"
#include "foundation/utility/autoreleaseptr.h"
#include "foundation/utility/iostreamop.h"
#include "foundation/utility/log.h"
#include "foundation/utility/searchpaths.h"
#include "foundation/utility/stopwatch.h"
#include "foundation/utility/string.h"

// appleseed.renderer headers.
#include "renderer/api/environment.h"
#include "renderer/api/frame.h"
#include "renderer/api/material.h"
#include "renderer/api/project.h"
#include "renderer/api/rendering.h"
#include "renderer/api/scene.h"
#include "renderer/api/utility.h"

// appleseed.maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exceptions.h"
#include "appleseedmaya/exporters/dagnodeexporter.h"
#include "appleseedmaya/exporters/exporterfactory.h"
#include "appleseedmaya/exporters/shadingengineexporter.h"
#include "appleseedmaya/exporters/shadingnetworkexporter.h"
#include "appleseedmaya/exporters/shapeexporter.h"
#include "appleseedmaya/idlejobqueue.h"
#include "appleseedmaya/logger.h"
#include "appleseedmaya/renderercontroller.h"
#include "appleseedmaya/renderglobalsnode.h"
#include "appleseedmaya/renderviewtilecallback.h"
#include "appleseedmaya/utils.h"

namespace bfs = boost::filesystem;
namespace asf = foundation;
namespace asr = renderer;

namespace AppleseedSession
{

Services::Services() {}
Services::~Services() {}

} // namespace AppleseedSession.

namespace
{

struct ScopedEndSession
{
    ~ScopedEndSession()
    {
        AppleseedSession::endSession();
    }
};

struct AbortRequested
{
};

class ScopedComputation
  : public NonCopyable
{
  public:
    ScopedComputation()
    {
        m_computation.beginComputation();
    }

    ~ScopedComputation()
    {
        m_computation.endComputation();
    }

    MComputation& computation()
    {
        return m_computation;
    }

    bool isInterruptRequested()
    {
        return m_computation.isInterruptRequested();
    }

    void thowIfInterruptRequested()
    {
        if (m_computation.isInterruptRequested())
            throw AbortRequested();
    }

    static MComputation m_computation;
};

MComputation ScopedComputation::m_computation;

struct SessionImpl
  : NonCopyable
{
    class ServicesImpl
      : public AppleseedSession::Services
    {
      public:
        ServicesImpl(SessionImpl& self)
          : Services()
          , m_self(self)
        {
        }

        virtual ShadingEngineExporterPtr createShadingEngineExporter(const MObject& object) const
        {
            MFnDependencyNode depNodeFn(object);

            ShadingEngineExporterMap::iterator it =
                m_self.m_shadingEngineExporters.find(depNodeFn.name());

            if (it != m_self.m_shadingEngineExporters.end())
                return it->second;

            ShadingEngineExporterPtr exporter(
                NodeExporterFactory::createShadingEngineExporter(
                    object,
                    *m_self.mainAssembly(),
                    m_self.m_sessionMode));
            m_self.m_shadingEngineExporters[depNodeFn.name()] = exporter;
            return exporter;
        }

        virtual ShadingNetworkExporterPtr createShadingNetworkExporter(
            const ShadingNetworkContext   context,
            const MObject&                object,
            const MPlug&                  outputPlug) const
        {
            MFnDependencyNode depNodeFn(object);

            ShadingNetworkExporterMap::iterator it =
                m_self.m_shadingNetworkExporters[context].find(depNodeFn.name());

            if (it != m_self.m_shadingNetworkExporters[context].end())
                return it->second;

            ShadingNetworkExporterPtr exporter(
                NodeExporterFactory::createShadingNetworkExporter(
                    context,
                    object,
                    outputPlug,
                    *m_self.mainAssembly(),
                    m_self.m_sessionMode));
            m_self.m_shadingNetworkExporters[context][depNodeFn.name()] = exporter;
            return exporter;
        }

        SessionImpl& m_self;
    };

    // Constructor. (IPR or Batch)
    SessionImpl(
        AppleseedSession::SessionMode       mode,
        const AppleseedSession::Options&    options,
        ScopedComputation*                  computation)
      : m_sessionMode(mode)
      , m_options(options)
      , m_services(*this)
      , m_computation(computation)
    {
        createProject();
    }

    // Constructor. (Scene export)
    SessionImpl(
        const MString&                      fileName,
        const AppleseedSession::Options&    options,
        ScopedComputation*                  computation)
      : m_sessionMode(AppleseedSession::ExportSession)
      , m_options(options)
      , m_services(*this)
      , m_computation(computation)
      , m_fileName(fileName)
    {
        m_projectPath = bfs::path(fileName.asChar()).parent_path();

        // Create a dir to store the geom files if it does not exist yet.
        boost::filesystem::path geomPath = m_projectPath / "_geometry";
        if (!boost::filesystem::exists(geomPath))
        {
            if (!boost::filesystem::create_directory(geomPath))
            {
                RENDERER_LOG_ERROR("Couldn't create geometry directory. Aborting");
                throw AppleseedSessionExportError();
            }
        }

        createProject();

        // Set the project filename and add the project directory to the search paths.
        m_project->set_path(m_fileName.asChar());
        m_project->search_paths().set_root_path(m_projectPath.string().c_str());
    }

    ~SessionImpl()
    {
        abortRender();
    }

    void createProject()
    {
        assert(m_project.get() == 0);

        m_project = asr::ProjectFactory::create("project");
        m_project->add_default_configurations();

        // Insert some config params needed by the interactive renderer.
        asr::Configuration *cfg = m_project->configurations().get_by_name("interactive");
        asr::ParamArray *cfg_params = &cfg->get_parameters();
        cfg_params->insert("sample_renderer", "generic");
        cfg_params->insert("sample_generator", "generic");
        cfg_params->insert("tile_renderer", "generic");
        cfg_params->insert("frame_renderer", "progressive");
        cfg_params->insert("lighting_engine", "pt");
        cfg_params->insert("pixel_renderer", "uniform");
        cfg_params->insert("sampling_mode", "qmc");
        cfg_params->insert_path("progressive_frame_renderer.max_fps", "5");

        // Insert some config params needed by the final renderer.
        cfg = m_project->configurations().get_by_name("final");
        cfg_params = &cfg->get_parameters();
        cfg_params->insert("sample_renderer", "generic");
        cfg_params->insert("sample_generator", "generic");
        cfg_params->insert("tile_renderer", "generic");
        cfg_params->insert("frame_renderer", "generic");
        cfg_params->insert("lighting_engine", "pt");
        cfg_params->insert("pixel_renderer", "uniform");
        cfg_params->insert("sampling_mode", "qmc");
        cfg_params->insert_path("uniform_pixel_renderer.samples", "16");

        // Create some basic project entities.

        // Create the frame.
        asf::auto_release_ptr<asr::Frame> frame(
            asr::FrameFactory::create("beauty", asr::ParamArray().insert("resolution", "640 480")));
        m_project->set_frame(frame);

        // 16 bits float (half) is the default pixel format in appleseed.
        // Force the pixel format to float to avoid half -> float conversions.
        m_project->get_frame()->get_parameters().insert("pixel_format", "float");

        // Create the scene
        asf::auto_release_ptr<asr::Scene> scene = asr::SceneFactory::create();
        m_project->set_scene(scene);

        // Create the environment.
        asf::auto_release_ptr<asr::Environment> environment(asr::EnvironmentFactory().create("environment", asr::ParamArray()));
        m_project->get_scene()->set_environment(environment);

        // Create the main assembly
        asf::auto_release_ptr<asr::Assembly> assembly = asr::AssemblyFactory().create("assembly", asr::ParamArray());
        m_project->get_scene()->assemblies().insert(assembly);

        // Instance the main assembly
        asf::auto_release_ptr<asr::AssemblyInstance> assemblyInstance = asr::AssemblyInstanceFactory::create("assembly_inst", asr::ParamArray(), "assembly");
        m_project->get_scene()->assembly_instances().insert(assemblyInstance);
    }

    void exportProject()
    {
        exportDefaultRenderGlobals();
        MObject globalsNode = exportAppleseedRenderGlobals();

        exportScene();

        // update the frame.
        {
            asr::ParamArray params = m_project->get_frame()->get_parameters();

            // Set the camera.
            if (m_options.m_camera.length() != 0)
            {
                MDagPath camera;
                MStatus status = getDagPathByName(m_options.m_camera, camera);

                // If the camera name is a transform name, move to the camera.
                status = camera.extendToShape();

                MFnDagNode fnDagNode(camera);
                if (fnDagNode.typeName() == "camera")
                {
                    RENDERER_LOG_DEBUG(
                        "Setting active camera to %s",
                        camera.fullPathName().asChar());
                    params.insert("camera", camera.fullPathName().asChar());
                }
                else
                    RENDERER_LOG_WARNING("Wrong camera!");
            }
            else
                RENDERER_LOG_WARNING("No active camera");

            // Set the resolution.
            params.insert("resolution", asf::Vector2i(m_options.m_width, m_options.m_height));

            // Set the tile size.
            MFnDependencyNode fnDepNode(globalsNode);
            int tileSize;
            if (AttributeUtils::get(fnDepNode, "tileSize", tileSize))
                params.insert("tile_size", asf::Vector2i(tileSize));

            // Replace the frame.
            m_project->set_frame(asr::FrameFactory().create("beauty", params));

            // Set the crop window.
            if (m_options.m_renderRegion)
            {
                m_project->get_frame()->set_crop_window(
                    asf::AABB2u(
                        asf::Vector2u(m_options.m_xmin, m_options.m_ymin),
                        asf::Vector2u(m_options.m_xmax, m_options.m_ymax)));
            }
        }
    }

    void exportScene()
    {
        createExporters();

        RENDERER_LOG_DEBUG("Collecting motion blur times");
        MotionBlurTimes motionBlurTimes;
        for(DagExporterMap::const_iterator it = m_dagExporters.begin(), e = m_dagExporters.end(); it != e; ++it)
            it->second->collectMotionBlurSteps(motionBlurTimes);

        checkUserAborted();

        // Create appleseed entities.
        RENDERER_LOG_DEBUG("Creating shading network entities");
        for(size_t i = 0; i < NumShadingNetworkContexts; ++i)
        {
            for(ShadingNetworkExporterMap::const_iterator it = m_shadingNetworkExporters[i].begin(), e = m_shadingNetworkExporters[i].end(); it != e; ++it)
                it->second->createEntities();
        }

        RENDERER_LOG_DEBUG("Creating shading engine entities");
        for(ShadingEngineExporterMap::const_iterator it = m_shadingEngineExporters.begin(), e = m_shadingEngineExporters.end(); it != e; ++it)
            it->second->createEntities(m_options);

        checkUserAborted();

        RENDERER_LOG_DEBUG("Creating dag entities");
        for(DagExporterMap::const_iterator it = m_dagExporters.begin(), e = m_dagExporters.end(); it != e; ++it)
            it->second->createEntities(m_options);

        RENDERER_LOG_DEBUG("Exporting motion steps");
        // For each time step...
        {
            for(DagExporterMap::const_iterator it = m_dagExporters.begin(), e = m_dagExporters.end(); it != e; ++it)
            {
                if (it->second->supportsMotionBlur())
                {
                    it->second->exportCameraMotionStep(0.0f);
                    it->second->exportTransformMotionStep(0.0f);
                    it->second->exportShapeMotionStep(0.0f);
                }

                checkUserAborted();
            }
        }

        // Handle auto-instancing.
        if (m_sessionMode != AppleseedSession::ProgressiveRenderSession)
        {
            RENDERER_LOG_DEBUG("Converting objects to instances");
            convertObjectsToInstances();
        }

        // Flush entities to the renderer.
        RENDERER_LOG_DEBUG("Flushing shading network entities");
        for(size_t i = 0; i < NumShadingNetworkContexts; ++i)
        {
            for(ShadingNetworkExporterMap::const_iterator it = m_shadingNetworkExporters[i].begin(), e = m_shadingNetworkExporters[i].end(); it != e; ++it)
                it->second->flushEntities();
        }

        checkUserAborted();

        RENDERER_LOG_DEBUG("Flushing shading engines entities");
        for(ShadingEngineExporterMap::const_iterator it = m_shadingEngineExporters.begin(), e = m_shadingEngineExporters.end(); it != e; ++it)
            it->second->flushEntities();

        checkUserAborted();

        RENDERER_LOG_DEBUG("Flushing dag entities");
        for(DagExporterMap::const_iterator it = m_dagExporters.begin(), e = m_dagExporters.end(); it != e; ++it)
            it->second->flushEntities();
    }

    void exportDefaultRenderGlobals()
    {
        RENDERER_LOG_DEBUG("Exporting default render globals");

        MObject defaultRenderGlobalsNode;
        if (getDependencyNodeByName("defaultRenderGlobals", defaultRenderGlobalsNode))
        {
            // todo: export globals here...
        }
    }

    MObject exportAppleseedRenderGlobals()
    {
        RENDERER_LOG_DEBUG("Exporting appleseed render globals");

        MObject appleseedRenderGlobalsNode;
        if (getDependencyNodeByName("appleseedRenderGlobals", appleseedRenderGlobalsNode))
        {
            RenderGlobalsNode::applyGlobalsToProject(
                appleseedRenderGlobalsNode,
                *m_project);
        }

        return appleseedRenderGlobalsNode;
    }

    void createExporters()
    {
        if (m_options.m_selectionOnly)
        {
            // Create exporters for the selected nodes in the scene.
            MStatus status;
            MSelectionList sel;
            status = MGlobal::getActiveSelectionList(sel);

            MDagPath rootPath, path;
            MItDag it(MItDag::kBreadthFirst);

            for(int i = 0, e = sel.length(); i < e; ++i)
            {
                status = sel.getDagPath(i, rootPath);
                if (status)
                {
                    for(it.reset(rootPath); !it.isDone(); it.next())
                    {
                        status = it.getPath(path);
                        if (status)
                            createDagNodeExporter(path);
                    }
                }
            }
        }
        else
        {
            // Create exporters for all the dag nodes in the scene.
            RENDERER_LOG_DEBUG("Creating dag node exporters");
            MDagPath path;
            for(MItDag it(MItDag::kDepthFirst); !it.isDone(); it.next())
            {
                it.getPath(path);
                createDagNodeExporter(path);
            }
        }

        createExtraExporters();
    }

    void createExtraExporters()
    {
        // Create dag extra exporters.
        RENDERER_LOG_DEBUG("Creating dag extra exporters");
        for(DagExporterMap::const_iterator it = m_dagExporters.begin(), e = m_dagExporters.end(); it != e; ++it)
            it->second->createExporters(m_services);

        checkUserAborted();

        // Create shading engine extra exporters.
        RENDERER_LOG_DEBUG("Creating shading engines extra exporters");
        for(ShadingEngineExporterMap::const_iterator it = m_shadingEngineExporters.begin(), e = m_shadingEngineExporters.end(); it != e; ++it)
            it->second->createExporters(m_services);
    }

    void createDagNodeExporter(const MDagPath& path)
    {
        checkUserAborted();

        if (m_dagExporters.count(path.fullPathName()) != 0)
            return;

        MFnDagNode dagNodeFn(path);

        // Avoid warnings about missing exporter for transform nodes.
        if (dagNodeFn.typeName() == "transform")
            return;

        DagNodeExporterPtr exporter;

        try
        {
            exporter.reset(NodeExporterFactory::createDagNodeExporter(
                path,
                *m_project,
                m_sessionMode));
        }
        catch (const NoExporterForNode&)
        {
            RENDERER_LOG_WARNING(
                "No dag exporter found for node type %s",
                dagNodeFn.typeName().asChar());
            return;
        }

        if (exporter)
        {
            m_dagExporters[path.fullPathName()] = exporter;
            RENDERER_LOG_DEBUG(
                "Created dag exporter for node %s",
                dagNodeFn.name().asChar());
        }
    }

    void convertObjectsToInstances()
    {
        for(DagExporterMap::const_iterator it = m_dagExporters.begin(), e = m_dagExporters.end(); it != e; ++it)
        {
            //const ShapeExporter *shape = dynamic_cast<const ShapeExporter*>(it->second.get());

            //if (shape && shape->supportsInstancing())
            //{
                /*
                hash = exporter->hash();
                if (instanceMap.find(hash) != end())
                {
                    DagNodeExporterPtr exporter(shape);
                    // Create InstanceExporter(...)
                    m_dagExporters[it->first] = instanceExporter;
                }
                else
                    map[hash] = ShapeExporterPtr(exporter)
                */
            //}
        }
    }

    void finalRender()
    {
        assert(MGlobal::mayaState() == MGlobal::kInteractive);
        assert(m_computation);

        IdleJobQueue::start();

        // Reset the renderer controller.
        m_rendererController.set_status(asr::IRendererController::ContinueRendering);

        // Create the master renderer.
        asr::Configuration *cfg = m_project->configurations().get_by_name("final");
        const asr::ParamArray& params = cfg->get_parameters();

        m_tileCallbackFactory.reset(
            new RenderViewTileCallbackFactory(m_rendererController, m_computation->computation()));
        m_tileCallbackFactory->renderViewStart(*m_project->get_frame());

        m_renderer.reset(
            new asr::MasterRenderer(
                *m_project,
                params,
                &m_rendererController,
                static_cast<asr::ITileCallbackFactory*>(m_tileCallbackFactory.get())));

        // Non blocking mode.
        boost::thread thread(&SessionImpl::renderFunc, this);
        m_renderThread.swap(thread);
    }

    void batchRender()
    {
        ScopedEndSession session;

        // Reset the renderer controller.
        m_rendererController.set_status(asr::IRendererController::ContinueRendering);

        // Create the master renderer.
        asr::Configuration *cfg = m_project->configurations().get_by_name("final");
        const asr::ParamArray& params = cfg->get_parameters();

        m_renderer.reset(
            new asr::MasterRenderer(
                *m_project,
                params,
                &m_rendererController,
                static_cast<asr::ITileCallbackFactory*>(0)));

        // todo: render here (blocking)...
        // todo: save frame here...
    }

    void progressiveRender()
    {
        /*
        // Reset the renderer controller.
        m_rendererController->set_status(asr::IRendererController::ContinueRendering);

        // Create the master renderer.
        asr::Configuration *cfg = m_project->configurations().get_by_name("interactive");
        const asr::ParamArray &params = cfg->get_parameters();

        RenderViewTileCallbackFactory tileCallbackFactory;
        m_renderer.reset( new asr::MasterRenderer(*m_project, params, m_rendererController, &tileCallbackFactory));
        */
    }

    void renderFunc()
    {
        m_renderer->render();
        IdleJobQueue::pushJob(&AppleseedSession::endSession);
    }

    void abortRender()
    {
        m_rendererController.set_status(asr::IRendererController::AbortRendering);
        if (m_renderThread.joinable())
            m_renderThread.join();
    }

    bool writeProject() const
    {
        return writeProject(m_fileName.asChar());
    }

    bool writeProject(const char *filename) const
    {
        return asr::ProjectFileWriter::write(
            *m_project,
            filename,
            asr::ProjectFileWriter::OmitHandlingAssetFiles |
            asr::ProjectFileWriter::OmitWritingGeometryFiles);
    }

    asr::Assembly *mainAssembly()
    {
        asr::Scene *scene = m_project->get_scene();
        return scene->assemblies().get_by_name("assembly");
    }

    void checkUserAborted() const
    {
        if (m_computation)
            m_computation->thowIfInterruptRequested();
    }

    typedef std::map<MString, DagNodeExporterPtr, MStringCompareLess>           DagExporterMap;
    typedef std::map<MString, ShadingEngineExporterPtr, MStringCompareLess>     ShadingEngineExporterMap;
    typedef std::map<MString, ShadingNetworkExporterPtr, MStringCompareLess>    ShadingNetworkExporterMap;
    typedef boost::array<ShadingNetworkExporterMap, NumShadingNetworkContexts>  ShadingNetworkExporterMapArray;

    AppleseedSession::SessionMode                           m_sessionMode;
    AppleseedSession::Options                               m_options;
    ScopedComputation*                                      m_computation;
    ServicesImpl                                            m_services;
    MTime                                                   m_savedTime;

    asf::auto_release_ptr<renderer::Project>                m_project;

    MString                                                 m_fileName;
    bfs::path                                               m_projectPath;

    DagExporterMap                                          m_dagExporters;
    ShadingEngineExporterMap                                m_shadingEngineExporters;
    ShadingNetworkExporterMapArray                          m_shadingNetworkExporters;

    boost::scoped_ptr<asr::MasterRenderer>                  m_renderer;
    RendererController                                      m_rendererController;
    asf::auto_release_ptr<RenderViewTileCallbackFactory>    m_tileCallbackFactory;

    boost::thread                                           m_renderThread;
};

// Globals.
bfs::path                       g_pluginPath;    // Plugin path.
MTime                           g_savedTime;     // Saved time.
boost::scoped_ptr<SessionImpl>  g_globalSession; // Global session.

} // unnamed

namespace AppleseedSession
{

MStatus initialize(const MString& pluginPath)
{
    g_pluginPath = pluginPath.asChar();
    return MS::kSuccess;
}

MStatus uninitialize()
{
    g_globalSession.reset();
    return MS::kSuccess;
}

MStatus projectExport(
    const MString& fileName,
    const Options& options)
{
    assert(g_globalSession.get() == 0);

    ScopedEndSession session;
    ScopedComputation computation;

    g_savedTime = MAnimControl::currentTime();

    if (options.m_sequence)
    {
        std::string fname = fileName.asChar();
        if (fname.find('#') == std::string::npos)
        {
            RENDERER_LOG_ERROR("No frame placeholders in filename.");
            return MS::kFailure;
        }

        for(int frame = options.m_firstFrame; frame <= options.m_lastFrame; frame += options.m_frameStep)
        {
            if (computation.isInterruptRequested())
            {
                RENDERER_LOG_INFO("Project export aborted.");
                return MS::kSuccess;
            }

            MGlobal::viewFrame(frame);
            fname = asf::get_numbered_string(fname, frame);
            try
            {
                g_globalSession.reset(new SessionImpl(fname.c_str(), options, &computation));
                g_globalSession->exportProject();
                g_globalSession->writeProject();
            }
            catch (const AbortRequested&)
            {
                RENDERER_LOG_INFO("Project export aborted.");
                return MS::kSuccess;
            }
            catch (const AppleseedMayaException&)
            {
                return MS::kFailure;
            }
        }
    }
    else
    {
        try
        {
            g_globalSession.reset(new SessionImpl(fileName, options, &computation));
            g_globalSession->exportProject();
            g_globalSession->writeProject();
        }
        catch (const AbortRequested&)
        {
            RENDERER_LOG_INFO("Project export aborted.");
            return MS::kSuccess;
        }
        catch (const AppleseedMayaException&)
        {
            return MS::kFailure;
        }
    }

    return MS::kSuccess;
}

MStatus finalRender(const Options& options, const bool batch)
{
    assert(g_globalSession.get() == 0);

    ScopedComputation computation;

    g_savedTime = MAnimControl::currentTime();

    try
    {
        g_globalSession.reset(new SessionImpl(FinalRenderSession, options, &computation));
        g_globalSession->exportProject();

        if (computation.isInterruptRequested())
            return MS::kSuccess;

        if (batch)
            g_globalSession->batchRender();
        else
            g_globalSession->finalRender();
    }
    catch (const AbortRequested&)
    {
        RENDERER_LOG_INFO("Render aborted.");
        return MS::kSuccess;
    }
    catch (const AppleseedMayaException&)
    {
        return MS::kFailure;
    }

    return MS::kSuccess;
}

void endSession()
{
    if (g_globalSession.get())
    {
        g_globalSession.reset();

        if (g_savedTime != MAnimControl::currentTime())
            MGlobal::viewFrame(g_savedTime);

        IdleJobQueue::stop();
    }
}

SessionMode sessionMode()
{
    if (g_globalSession.get() == 0)
        return NoSession;

    return g_globalSession->m_sessionMode;
}

const Options& options()
{
    assert(g_globalSession.get());

    return g_globalSession->m_options;
}

} // namespace AppleseedSession.
