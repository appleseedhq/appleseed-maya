
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016-2017 Esteban Tovagliari, The appleseedhq Organization
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
#include <array>
#include <memory>
#include <thread>
#include <vector>

// Boost headers.
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/convenience.hpp"
#include "boost/filesystem/operations.hpp"

// Maya headers.
#include <maya/MAnimControl.h>
#include <maya/MCommonRenderSettingsData.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnRenderLayer.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MSelectionList.h>
#include <maya/MObject.h>
#include <maya/MObjectArray.h>
#include <maya/MRenderUtil.h>
#include "appleseedmaya/mayaheaderscleanup.h"

// appleseed.foundation headers.
#include "foundation/math/scalar.h"
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
#include "appleseedmaya/exporters/alphamapexporter.h"
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

namespace bfs = boost::filesystem;
namespace asf = foundation;
namespace asr = renderer;

namespace AppleseedSession
{

Options::Options()
  : m_selectionOnly(false)
  , m_width(-1)
  , m_height(-1)
  , m_renderRegion(false)
  , m_xmin(-1)
  , m_ymin(-1)
  , m_xmax(-1)
  , m_ymax(-1)
  , m_colorspace("linear_rgb")
  , m_sequence(false)
  , m_firstFrame(1)
  , m_lastFrame(1)
  , m_frameStep(1)
{
}

MotionBlurTimes::MotionBlurTimes()
  : m_shutterOpenTime(0.0f)
  , m_shutterCloseTime(0.0f)
{
}

void MotionBlurTimes::clear()
{
    m_cameraTimes.clear();
    m_transformTimes.clear();
    m_deformTimes.clear();
    m_allTimes.clear();
}

void MotionBlurTimes::initializeToCurrentFrame()
{
    const float now = static_cast<float>(MAnimControl::currentTime().value());
    m_shutterOpenTime = now;
    m_shutterCloseTime = now;

    clear();
    m_cameraTimes.insert(now);
    m_transformTimes.insert(now);
    m_deformTimes.insert(now);
    m_allTimes.insert(now);
}

void MotionBlurTimes::initializeFrameSet(
    const size_t        numSamples,
    const float         shutterOpenTime,
    const float         shutterCloseTime,
    std::set<float>&    times)
{
    times.clear();

    if (numSamples == 1)
        times.insert(shutterOpenTime);
    else
    {
        const size_t one = 1;
        for (size_t i = 1; i <= numSamples; ++i)
            times.insert(asf::fit(i, one, numSamples, shutterOpenTime, shutterCloseTime));
    }
}

void MotionBlurTimes::mergeTimes()
{
    m_allTimes.clear();
    m_allTimes.insert(m_cameraTimes.begin(), m_cameraTimes.end());
    m_allTimes.insert(m_transformTimes.begin(), m_transformTimes.end());
    m_allTimes.insert(m_deformTimes.begin(), m_deformTimes.end());
}

float MotionBlurTimes::normalizedFrame(float frame) const
{
    if (m_shutterCloseTime == m_shutterOpenTime)
        return 0.0f;

    return (frame - m_shutterOpenTime) / (m_shutterCloseTime - m_shutterOpenTime);
}

Services::Services() = default;
Services::~Services() = default;

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

        ShadingEngineExporterPtr createShadingEngineExporter(const MObject& object) const override
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

        ShadingNetworkExporterPtr createShadingNetworkExporter(
            const ShadingNetworkContext   context,
            const MObject&                object,
            const MPlug&                  outputPlug) const override
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

        AlphaMapExporterPtr createAlphaMapExporter(
            const MObject&                  object) const override
        {
            MFnDependencyNode depNodeFn(object);

            AlphaMapExporterMap::iterator it =
                m_self.m_alphaMapExporters.find(depNodeFn.name());

            if (it != m_self.m_alphaMapExporters.end())
                return it->second;

            AlphaMapExporterPtr exporter(
                NodeExporterFactory::createAlphaMapExporter(
                    object,
                    *m_self.m_project,
                    m_self.m_sessionMode));

            if (exporter)
                m_self.m_alphaMapExporters[depNodeFn.name()] = exporter;

            return exporter;
        }

        SessionImpl& m_self;
    };

    // Constructor. (IPR or Batch)
    SessionImpl(
        AppleseedSession::SessionMode       mode,
        const AppleseedSession::Options&    options,
        ComputationPtr                      computation)
      : m_sessionMode(mode)
      , m_options(options)
      , m_services(*this)
      , m_computation(computation)
    {
        createProject(options.m_colorspace);
    }

    // Constructor. (Scene export)
    SessionImpl(
        const MString&                      fileName,
        const AppleseedSession::Options&    options,
        ComputationPtr                      computation)
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

        createProject(options.m_colorspace);

        // Set the project filename and add the project directory to the search paths.
        m_project->set_path(m_fileName.asChar());
        m_project->search_paths().set_root_path(m_projectPath.string().c_str());
    }

    ~SessionImpl()
    {
        abortRender();
    }

    void createProject(const char* colorspace)
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
            asr::FrameFactory::create(
                "beauty",
                asr::ParamArray()
                    .insert("resolution", "640 480")
                    .insert("pixel_format", "float")
                    .insert("color_space", colorspace)));
        m_project->set_frame(frame);

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

        AppleseedSession::MotionBlurTimes motionBlurTimes;
        // Only do motion blur for non progressive renders.
        if (m_sessionMode != AppleseedSession::ProgressiveRenderSession)
            RenderGlobalsNode::collectMotionBlurTimes(globalsNode, motionBlurTimes);
        else
            motionBlurTimes.initializeToCurrentFrame();

        exportScene(motionBlurTimes);

        // Set the shutter open and close times in all cameras.
        asr::CameraContainer& cameras = m_project->get_scene()->cameras();
        for (size_t i = 0, e = cameras.size(); i < e; ++i)
        {
            cameras.get_by_index(i)->get_parameters()
                .insert("shutter_open_time", motionBlurTimes.normalizedFrame(motionBlurTimes.m_shutterOpenTime))
                .insert("shutter_close_time", motionBlurTimes.normalizedFrame(motionBlurTimes.m_shutterCloseTime));
        }

        asr::ParamArray params = m_project->get_frame()->get_parameters();

        // Set the camera.
        if (m_options.m_camera.length() != 0)
        {
            MDagPath camera;
            MStatus status = getDagPathByName(m_options.m_camera, camera);

            // If the camera name is a transform name, move to the camera.
            status = camera.extendToShape();

            MFnDagNode dagNodeFn(camera);
            if (dagNodeFn.typeName() == "camera")
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
            RENDERER_LOG_WARNING("No active camera in project!");

        // Set the environment.
        if (!m_project->get_scene()->environment_edfs().empty())
        {
            MObject appleseedRenderGlobalsNode;
            getDependencyNodeByName("appleseedRenderGlobals", appleseedRenderGlobalsNode);
            MFnDependencyNode depNodeFn(appleseedRenderGlobalsNode);
            MPlug dstPlug = depNodeFn.findPlug("envLight");
            MPlug srcPlug;

            if (AttributeUtils::getPlugConnectedTo(dstPlug, srcPlug))
            {
                if (!srcPlug.isNull())
                {
                    MFnDagNode dagNodeFn(srcPlug.node());
                    const MString envName = dagNodeFn.fullPathName();

                    m_project->get_scene()->get_environment()->get_parameters()
                        .insert("environment_edf", envName.asChar());

                    bool bgLight;
                    AttributeUtils::get(appleseedRenderGlobalsNode, "bgLight", bgLight);
                    if (bgLight)
                    {
                        const MString envShaderName = envName + "_shader";
                        m_project->get_scene()->get_environment()->get_parameters()
                            .insert("environment_shader", envShaderName.asChar());
                    }
                }
            }
        }

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

    void exportScene(const AppleseedSession::MotionBlurTimes& motionBlurTimes)
    {
        createExporters();
        checkUserAborted();

        RENDERER_LOG_DEBUG("Creating alpha map entities");
        for(AlphaMapExporterMap::const_iterator it = m_alphaMapExporters.begin(), e = m_alphaMapExporters.end(); it != e; ++it)
            it->second->createEntities();

        checkUserAborted();

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
            it->second->createEntities(m_options, motionBlurTimes);

        RENDERER_LOG_DEBUG("Exporting motion steps");
        std::set<float>::const_iterator frameIt(motionBlurTimes.m_allTimes.begin());
        std::set<float>::const_iterator frameEnd(motionBlurTimes.m_allTimes.end());
        for (; frameIt != frameEnd; ++frameIt)
        {
            const float now = static_cast<float>(MAnimControl::currentTime().value());

            if (*frameIt != now)
            {
                RENDERER_LOG_DEBUG("Setting frame to %d", *frameIt);
                MGlobal::viewFrame(*frameIt);
            }

            const float frame = motionBlurTimes.normalizedFrame(*frameIt);

            for(DagExporterMap::const_iterator it = m_dagExporters.begin(), e = m_dagExporters.end(); it != e; ++it)
            {
                if (it->second->supportsMotionBlur())
                {
                    if (motionBlurTimes.m_cameraTimes.count(*frameIt))
                        it->second->exportCameraMotionStep(frame);

                    if (motionBlurTimes.m_transformTimes.count(*frameIt))
                        it->second->exportTransformMotionStep(frame);

                    if (motionBlurTimes.m_deformTimes.count(*frameIt))
                        it->second->exportShapeMotionStep(frame);
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

        checkUserAborted();

        RENDERER_LOG_DEBUG("Flushing alpha map entities");
        for(AlphaMapExporterMap::const_iterator it = m_alphaMapExporters.begin(), e = m_alphaMapExporters.end(); it != e; ++it)
            it->second->flushEntities();

        checkUserAborted();

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
                    instanceMap[hash] = ShapeExporterPtr(exporter)
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
            new RenderViewTileCallbackFactory(m_rendererController, m_computation));
        m_tileCallbackFactory->renderViewStart(*m_project->get_frame());

        m_renderer.reset(
            new asr::MasterRenderer(
                *m_project,
                params,
                &m_rendererController,
                static_cast<asr::ITileCallbackFactory*>(m_tileCallbackFactory.get())));

        // Non blocking mode.
        std::thread thread(&SessionImpl::renderFunc, this);
        m_renderThread.swap(thread);
    }

    void batchRender()
    {
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

        m_renderer->render();
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

    void writeMainImage(const char *filename) const
    {
        const asr::Frame* frame = m_project->get_frame();
        frame->write_main_image(filename);
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
    typedef std::array<ShadingNetworkExporterMap, NumShadingNetworkContexts>    ShadingNetworkExporterMapArray;
    typedef std::map<MString, AlphaMapExporterPtr, MStringCompareLess>          AlphaMapExporterMap;

    AppleseedSession::SessionMode                           m_sessionMode;
    AppleseedSession::Options                               m_options;
    ComputationPtr                                          m_computation;
    ServicesImpl                                            m_services;
    MTime                                                   m_savedTime;

    asf::auto_release_ptr<renderer::Project>                m_project;

    MString                                                 m_fileName;
    bfs::path                                               m_projectPath;

    DagExporterMap                                          m_dagExporters;
    ShadingEngineExporterMap                                m_shadingEngineExporters;
    ShadingNetworkExporterMapArray                          m_shadingNetworkExporters;
    AlphaMapExporterMap                                     m_alphaMapExporters;

    std::unique_ptr<asr::MasterRenderer>                    m_renderer;
    RendererController                                      m_rendererController;
    asf::auto_release_ptr<RenderViewTileCallbackFactory>    m_tileCallbackFactory;

    std::thread                                             m_renderThread;
};

// Globals.
bfs::path                       g_pluginPath;    // Plugin path.
MTime                           g_savedTime;     // Saved time.
std::unique_ptr<SessionImpl>    g_globalSession; // Global session.

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

namespace
{

void beginSession(
    AppleseedSession::SessionMode       mode,
    const AppleseedSession::Options&    options,
    ComputationPtr                      computation)
{
    g_globalSession.reset(new SessionImpl(mode, options, computation));
}

void beginSession(
    const char*                         fileName,
    const AppleseedSession::Options&    options,
    ComputationPtr                      computation)
{
    g_globalSession.reset(new SessionImpl(fileName, options, computation));
}

} // unamed

MStatus projectExport(
    const MString& fileName,
    Options        options)
{
    // In case we were doing IPR.
    endSession();

    ScopedEndSession session;
    ComputationPtr computation = Computation::create();

    g_savedTime = MAnimControl::currentTime();

    if (options.m_sequence)
    {
        std::string fname_template = fileName.asChar();
        if (fname_template.find('#') == std::string::npos)
        {
            RENDERER_LOG_ERROR("No frame placeholders in filename.");
            return MS::kFailure;
        }

        for(int frame = options.m_firstFrame; frame <= options.m_lastFrame; frame += options.m_frameStep)
        {
            if (computation->isInterruptRequested())
            {
                RENDERER_LOG_INFO("Project export aborted.");
                return MS::kSuccess;
            }

            MGlobal::viewFrame(frame);
            const std::string fname = asf::get_numbered_string(fname_template, frame);
            try
            {
                beginSession(fname.c_str(), options, computation);
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
            beginSession(fileName.asChar(), options, computation);
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

MStatus render(Options options)
{
    // In case we were doing IPR.
    endSession();

    ComputationPtr computation = Computation::create();

    g_savedTime = MAnimControl::currentTime();

    try
    {
        beginSession(FinalRenderSession, options, computation);
        g_globalSession->exportProject();

        if (computation->isInterruptRequested())
            return MS::kSuccess;

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

namespace
{

MString batchRenderFileName(
    const MCommonRenderSettingsData&    renderSettings,
    double                              frame,
    const MString&                      sceneName,
    const MString&                      cameraName,
    const MString&                      fileFormat,
    MObject                             renderLayer,
    MStatus*                            status)
{
    return renderSettings.getImageName(
        MCommonRenderSettingsData::kFullPathImage,
        frame,
        sceneName,
        cameraName,
        fileFormat,
        renderLayer,
        true,
        status);
}

MStatus batchRenderFrame(
    Options        options,
    const MString& outputFilename)
{
    ScopedEndSession session;

    try
    {
        if (asf::ends_with(outputFilename.asChar(), ".png"))
            options.m_colorspace = "srgb";
        else
            options.m_colorspace = "linear_rgb";

        beginSession(FinalRenderSession, options, ComputationPtr());
        g_globalSession->exportProject();
        g_globalSession->batchRender();
        g_globalSession->writeMainImage(outputFilename.asChar());
    }
    catch (const AppleseedMayaException&)
    {
        return MS::kFailure;
    }
    catch (const std::exception&)
    {
        return MS::kFailure;
    }
    catch (...)
    {
        return MS::kFailure;
    }

    return MS::kSuccess;
}

} // unnamed.

MStatus batchRender(Options options)
{
    // In case we were doing IPR.
    endSession();

    MStatus status;

    // Hack!: update the file format and file extension in the render globals.
    MGlobal::executePythonCommand(
        "import appleseedMaya.renderGlobals\n"
        "appleseedMaya.renderGlobals.imageFormatChanged()");

    // Get the current scene name.
    MString sceneName;
    MGlobal::executeCommand("file -q -sn -shn", sceneName);

    if (sceneName.length() != 0)
    {
        // Filename of the scene without the __xxxx tmp file suffix
        // and without the .ma or .mb extension.
        sceneName = MString(sceneName.asChar(), sceneName.length() - 9);
    }
    else
        sceneName.set("untitled");

    // Use the first renderable camera as the render camera.
    MString cameraName;
    {
        MDagPath path;
        for(MItDag it(MItDag::kDepthFirst, MFn::kCamera); !it.isDone(); it.next())
        {
            it.getPath(path);
            MFnDagNode dagNodeFn(path);

            bool isRenderable = false;
            AttributeUtils::get(path.node(), "renderable", isRenderable);

            if (isRenderable)
            {
                options.m_camera = dagNodeFn.fullPathName();
                cameraName = dagNodeFn.name();
                break;
            }
        }
    }

    // TODO: check if this is needed and how to get it...
    MString fileFormat;

    MObject renderLayer = MFnRenderLayer::currentLayer(&status);

    MCommonRenderSettingsData renderSettings;
    MRenderUtil::getCommonRenderSettings(renderSettings);

    if (renderSettings.isAnimated())
    {
        const double frameStart = renderSettings.frameStart.value();
        const double frameEnd = renderSettings.frameEnd.value();
        const double frameBy = renderSettings.frameBy;

        for (double frame = frameStart; frame <= frameEnd; frame += frameBy)
        {
            MGlobal::viewFrame(frame);
            MString outputFileName = batchRenderFileName(
                renderSettings,
                frame,
                sceneName,
                cameraName,
                fileFormat,
                renderLayer,
                &status);

            RENDERER_LOG_DEBUG("Batch render: rendering frame %f, filename = %s", frame, outputFileName.asChar());
            status = batchRenderFrame(options, outputFileName);
            RENDERER_LOG_DEBUG("Status = %s", status.errorString().asChar());
            RENDERER_LOG_DEBUG("=================================");
        }
    }
    else
    {
        const double frame = MAnimControl::currentTime().value();
        MString outputFileName = batchRenderFileName(
            renderSettings,
            frame,
            sceneName,
            cameraName,
            fileFormat,
            renderLayer,
            &status);

        RENDERER_LOG_DEBUG("Batch render: rendering single frame, filename = %s", outputFileName.asChar());
        status = batchRenderFrame(options, outputFileName);
        RENDERER_LOG_DEBUG("Status = %s", status.errorString().asChar());
        RENDERER_LOG_DEBUG("=================================");
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
