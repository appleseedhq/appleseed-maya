
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016-2019 Esteban Tovagliari, The appleseedhq Organization
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
#include "appleseedsession.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exceptions.h"
#include "appleseedmaya/exporters/alphamapexporter.h"
#include "appleseedmaya/exporters/dagnodeexporter.h"
#include "appleseedmaya/exporters/exporterfactory.h"
#include "appleseedmaya/exporters/instanceexporter.h"
#include "appleseedmaya/exporters/shadingengineexporter.h"
#include "appleseedmaya/exporters/shadingnetworkexporter.h"
#include "appleseedmaya/exporters/shapeexporter.h"
#include "appleseedmaya/idlejobqueue.h"
#include "appleseedmaya/logger.h"
#include "appleseedmaya/pythonbridge.h"
#include "appleseedmaya/renderercontroller.h"
#include "appleseedmaya/renderglobalsnode.h"
#include "appleseedmaya/renderviewtilecallback.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/aov.h"
#include "renderer/api/environment.h"
#include "renderer/api/frame.h"
#include "renderer/api/material.h"
#include "renderer/api/project.h"
#include "renderer/api/rendering.h"
#include "renderer/api/scene.h"
#include "renderer/api/shadergroup.h"
#include "renderer/api/utility.h"

// appleseed.foundation headers.
#include "foundation/log/log.h"
#include "foundation/math/scalar.h"
#include "foundation/memory/autoreleaseptr.h"
#include "foundation/platform/timers.h"
#include "foundation/string/string.h"
#include "foundation/utility/iostreamop.h"
#include "foundation/utility/searchpaths.h"
#include "foundation/utility/stopwatch.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
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
#include "appleseedmaya/_endmayaheaders.h"

// Boost headers.
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/convenience.hpp"
#include "boost/filesystem/operations.hpp"

// Standard headers.
#include <array>
#include <cassert>
#include <fstream>
#include <memory>
#include <thread>
#include <vector>

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
  , m_sequence(false)
  , m_firstFrame(1)
  , m_lastFrame(1)
  , m_frameStep(1)
  , m_writeBoundingBox(false)
{
}

MotionBlurSampleTimes::MotionBlurSampleTimes()
  : m_shutterOpenTime(0.0f)
  , m_shutterCloseTime(0.0f)
{
}

void MotionBlurSampleTimes::clear()
{
    m_cameraTimes.clear();
    m_transformTimes.clear();
    m_deformTimes.clear();
    m_allTimes.clear();
}

void MotionBlurSampleTimes::initializeToCurrentFrame()
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

void MotionBlurSampleTimes::initializeFrameSet(
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

void MotionBlurSampleTimes::mergeTimes()
{
    m_allTimes.clear();
    m_allTimes.insert(m_cameraTimes.begin(), m_cameraTimes.end());
    m_allTimes.insert(m_transformTimes.begin(), m_transformTimes.end());
    m_allTimes.insert(m_deformTimes.begin(), m_deformTimes.end());
}

float MotionBlurSampleTimes::normalizedFrame(float frame) const
{
    if (m_shutterCloseTime == m_shutterOpenTime)
        return 0.0f;

    return (frame - m_shutterOpenTime) / (m_shutterCloseTime - m_shutterOpenTime);
}

IExporterFactory::IExporterFactory()
{
}

IExporterFactory::~IExporterFactory()
{
}

} // namespace AppleseedSession.

namespace
{
    struct SessionImpl;

    // Globals.
    bfs::path                       g_pluginPath;             // Plugin path.
    asf::SearchPaths                g_resourceSearchPaths;    // Paths to resources.
    MTime                           g_savedTime;              // Saved time.
    asf::LogMessage::Category       g_savedLogLevel;          // Saved log level.
    std::unique_ptr<SessionImpl>    g_globalSession;          // Global session.

    // RAII class to end active the session in an exception safe way.
    struct ScopedEndSession
    {
        ~ScopedEndSession()
        {
            AppleseedSession::endSession();
        }
    };

    struct SessionImpl
      : public asf::NonCopyable
    {
        class ExporterFactory
          : public AppleseedSession::IExporterFactory
        {
          public:
            ExporterFactory(SessionImpl& self)
              : IExporterFactory()
              , m_self(self)
            {
            }

            ShadingEngineExporterPtr createShadingEngineExporter(const MObject& object) const override
            {
                MFnDependencyNode depNodeFn(object);

                auto it = m_self.m_shadingEngineExporters.find(depNodeFn.name());

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

                auto it = m_self.m_shadingNetworkExporters[context].find(depNodeFn.name());

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

                auto it = m_self.m_alphaMapExporters.find(depNodeFn.name());

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

        // Constructor (IPR or batch render).
        SessionImpl(
            AppleseedSession::SessionMode       mode,
            const AppleseedSession::Options&    options,
            ComputationPtr                      computation)
          : m_sessionMode(mode)
          , m_options(options)
          , m_computation(computation)
          , m_exporter_factory(*this)
        {
            createProject();
        }

        // Constructor (scene export).
        SessionImpl(
            const MString&                      fileName,
            const AppleseedSession::Options&    options,
            ComputationPtr                      computation)
          : m_sessionMode(AppleseedSession::ExportSession)
          , m_options(options)
          , m_computation(computation)
          , m_exporter_factory(*this)
          , m_fileName(fileName)
        {
            m_projectPath = bfs::path(fileName.asChar()).parent_path();

            // Create a directory to store the geometry files if it does not exist yet.
            bfs::path geomPath = m_projectPath / "_geometry";
            if (!bfs::exists(geomPath))
            {
                if (!bfs::create_directory(geomPath))
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
            PythonBridge::clearCurrentProject();
            abortRender();
        }

        void initializeConfiguration(asr::ParamArray& params) const
        {
            params.insert("sample_renderer", "generic");
            params.insert("sample_generator", "generic");
            params.insert("tile_renderer", "generic");
            params.insert("pixel_renderer", "uniform");
            params.insert("sampling_mode", "qmc");
            params.insert("lighting_engine", "pt");
            params.insert("spectrum_mode", "rgb");
        }

        void createProject()
        {
            assert(m_project.get() == 0);

            m_project = asr::ProjectFactory::create("project");
            m_project->add_default_configurations();

            // Make the project available from Python.
            PythonBridge::setCurrentProject(m_project.get());

            // Insert some config params needed by the interactive renderer.
            asr::Configuration* cfg = m_project->configurations().get_by_name("interactive");
            asr::ParamArray* configParams = &cfg->get_parameters();
            initializeConfiguration(*configParams);
            configParams->insert("frame_renderer", "progressive");
            configParams->insert_path("progressive_frame_renderer.max_fps", "5");

            // Insert some config params needed by the final renderer.
            cfg = m_project->configurations().get_by_name("final");
            configParams = &cfg->get_parameters();
            initializeConfiguration(*configParams);
            configParams->insert("frame_renderer", "generic");
            configParams->insert_path("uniform_pixel_renderer.samples", "32");
            configParams->insert_path("adaptive_tile_renderer.min_samples", "0");
            configParams->insert_path("adaptive_tile_renderer.max_samples", "32");
            configParams->insert_path("adaptive_tile_renderer.batch_size", "16");
            configParams->insert_path("adaptive_tile_renderer.noise_threshold", "1.0");

            // Create some basic project entities.

            // Create the frame.
            asf::auto_release_ptr<asr::Frame> frame(
                asr::FrameFactory::create(
                    "beauty",
                    asr::ParamArray().insert("resolution", "640 480")));
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
            asf::auto_release_ptr<asr::AssemblyInstance> assemblyInstance = asr::AssemblyInstanceFactory::create(
                "assembly_inst",
                asr::ParamArray(),
                "assembly");
            m_project->get_scene()->assembly_instances().insert(assemblyInstance);
        }

        void exportProject()
        {
            exportDefaultRenderGlobals();
            MObject globalsNode = exportAppleseedRenderGlobals();

            AppleseedSession::MotionBlurSampleTimes motionBlurSampleTimes;

            // Only do motion blur for non progressive renders.
            if (m_sessionMode != AppleseedSession::ProgressiveRenderSession)
                RenderGlobalsNode::collectMotionBlurSampleTimes(globalsNode, motionBlurSampleTimes);
            else
                motionBlurSampleTimes.initializeToCurrentFrame();

            exportScene(motionBlurSampleTimes);

            // Set the shutter open and close times in all cameras.
            asr::CameraContainer& cameras = m_project->get_scene()->cameras();

            const float shutterOpenTime = motionBlurSampleTimes.normalizedFrame(motionBlurSampleTimes.m_shutterOpenTime);
            const float shutterCloseTime = motionBlurSampleTimes.normalizedFrame(motionBlurSampleTimes.m_shutterCloseTime);

            for (size_t i = 0, e = cameras.size(); i < e; ++i)
            {
                cameras.get_by_index(i)->get_parameters()
                    .insert("shutter_open_begin_time", shutterOpenTime)
                    .insert("shutter_open_end_time", shutterOpenTime)
                    .insert("shutter_close_begin_time", shutterCloseTime)
                    .insert("shutter_close_end_time", shutterCloseTime);
            }

            asr::ParamArray params = m_project->get_frame()->get_parameters();

            // Set the camera.
            if (m_options.m_camera.length() != 0)
            {
                MDagPath camera;
                MStatus status = getDagPathByName(m_options.m_camera, camera);

                // If the camera is a transform, move to the shape.
                status = camera.extendToShape();

                MFnDagNode dagNodeFn(camera);
                if (dagNodeFn.typeName() == "camera")
                {
                    RENDERER_LOG_DEBUG(
                        "Setting active camera to %s",
                        camera.partialPathName().asChar());
                    params.insert("camera", camera.partialPathName().asChar());
                }
                else
                {
                    RENDERER_LOG_WARNING(
                        "Invalid camera specified %s",
                        camera.partialPathName().asChar());
                }
            }
            else
                RENDERER_LOG_WARNING("No active camera in project");

            // Set the environment.
            if (!m_project->get_scene()->environment_edfs().empty())
            {
                // Get the appleseed globals node.
                MObject appleseedRenderGlobalsNode;
                getDependencyNodeByName("appleseedRenderGlobals", appleseedRenderGlobalsNode);
                MFnDependencyNode depNodeFn(appleseedRenderGlobalsNode);
                MPlug dstPlug = depNodeFn.findPlug("envLight", /*wantNetworkedPlug=*/ false);
                MPlug srcPlug;

                // Find the environment light connected to the globals node.
                if (AttributeUtils::getPlugConnectedTo(dstPlug, srcPlug))
                {
                    if (!srcPlug.isNull())
                    {
                        MFnDagNode dagNodeFn(srcPlug.node());
                        const MString envName = dagNodeFn.partialPathName();

                        // Set the active environment.
                        m_project->get_scene()->get_environment()->get_parameters()
                            .insert("environment_edf", envName.asChar());

                        // Make the environment visible if the user set it.
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

            MFnDependencyNode fnDepNode(globalsNode);

            // Apply the scene scale factor.
            float sceneScale;
            if (AttributeUtils::get(fnDepNode, "sceneScale", sceneScale))
            {
                if (sceneScale != 1.0f)
                {
                    asr::TransformSequence scaleTransformSeq;
                    scaleTransformSeq.set_transform(0.0, asf::Transformd::from_local_to_parent(
                        asf::Matrix4d::make_scaling(asf::Vector3d(sceneScale))));

                    // Scale the main assembly instance.
                    asr::Scene* scene = m_project->get_scene();
                    asr::AssemblyInstance* assemblyInstance =
                        scene->assembly_instances().get_by_name("assembly_inst");
                    assemblyInstance->transform_sequence() = scaleTransformSeq;

                    // Apply the scale to all cameras.
                    for (size_t i = 0, e = scene->cameras().size(); i < e; ++i)
                    {
                        asr::Camera* camera = scene->cameras().get_by_index(i);
                        camera->transform_sequence() = camera->transform_sequence() * scaleTransformSeq;
                    }
                }
            }

            // Set the resolution.
            params.insert("resolution", asf::Vector2i(m_options.m_width, m_options.m_height));

            // Replace the frame and apply post processing stages.
            m_project->set_frame(asr::FrameFactory().create("beauty", params, m_aovs));
            RenderGlobalsNode::applyPostProcessStagesToFrame(globalsNode, *m_project);

            // Set the crop window.
            if (m_options.m_renderRegion)
            {
                m_project->get_frame()->set_crop_window(
                    asf::AABB2u(
                        asf::Vector2u(m_options.m_xmin, m_options.m_ymin),
                        asf::Vector2u(m_options.m_xmax, m_options.m_ymax)));
            }
        }

        bool autoInstancingEnabled() const
        {
            // When doing interactive rendering, we disable auto-instancing.
            if (m_sessionMode == AppleseedSession::ProgressiveRenderSession)
                return false;

            return true;
        }

        void exportScene(const AppleseedSession::MotionBlurSampleTimes& motionBlurSampleTimes)
        {
            createExporters();
            throwIfUserAborted();

            RENDERER_LOG_DEBUG("Creating alpha map entities");
            for (auto it = m_alphaMapExporters.begin(), e = m_alphaMapExporters.end(); it != e; ++it)
                it->second->createEntities();

            throwIfUserAborted();

            RENDERER_LOG_DEBUG("Creating shading network entities");
            for (size_t i = 0; i < NumShadingNetworkContexts; ++i)
            {
                for (auto it = m_shadingNetworkExporters[i].begin(), e = m_shadingNetworkExporters[i].end(); it != e; ++it)
                    it->second->createEntities();
            }

            RENDERER_LOG_DEBUG("Creating shading engine entities");
            for (auto it = m_shadingEngineExporters.begin(), e = m_shadingEngineExporters.end(); it != e; ++it)
                it->second->createEntities(m_options);

            throwIfUserAborted();

            RENDERER_LOG_DEBUG("Creating dag entities");
            for (auto it = m_dagExporters.begin(), e = m_dagExporters.end(); it != e; ++it)
                it->second->createEntities(m_options, motionBlurSampleTimes);

            RENDERER_LOG_DEBUG("Exporting motion steps");
            auto frameIt(motionBlurSampleTimes.m_allTimes.begin());
            auto frameEnd(motionBlurSampleTimes.m_allTimes.end());
            for (; frameIt != frameEnd; ++frameIt)
            {
                const float now = static_cast<float>(MAnimControl::currentTime().value());

                if (*frameIt != now)
                {
                    RENDERER_LOG_DEBUG("Setting frame to %f", *frameIt);
                    MGlobal::viewFrame(*frameIt);
                }

                const float frame = motionBlurSampleTimes.normalizedFrame(*frameIt);

                for (auto it = m_dagExporters.begin(), e = m_dagExporters.end(); it != e; ++it)
                {
                    if (it->second->supportsMotionBlur())
                    {
                        if (motionBlurSampleTimes.m_cameraTimes.count(*frameIt))
                            it->second->exportCameraMotionStep(frame);

                        if (motionBlurSampleTimes.m_transformTimes.count(*frameIt))
                            it->second->exportTransformMotionStep(frame);

                        if (motionBlurSampleTimes.m_deformTimes.count(*frameIt))
                            it->second->exportShapeMotionStep(frame);
                    }

                    throwIfUserAborted();
                }
            }

            if (autoInstancingEnabled())
            {
                RENDERER_LOG_DEBUG("Converting objects to instances");
                convertObjectsToInstances();
            }

            throwIfUserAborted();

            RENDERER_LOG_DEBUG("Flushing alpha map entities");
            for (auto it = m_alphaMapExporters.begin(), e = m_alphaMapExporters.end(); it != e; ++it)
                it->second->flushEntities();

            throwIfUserAborted();

            RENDERER_LOG_DEBUG("Flushing shading network entities");
            for (size_t i = 0; i < NumShadingNetworkContexts; ++i)
            {
                for (auto it = m_shadingNetworkExporters[i].begin(), e = m_shadingNetworkExporters[i].end(); it != e; ++it)
                    it->second->flushEntities();
            }

            throwIfUserAborted();

            RENDERER_LOG_DEBUG("Flushing shading engines entities");
            for (auto it = m_shadingEngineExporters.begin(), e = m_shadingEngineExporters.end(); it != e; ++it)
                it->second->flushEntities();

            throwIfUserAborted();

            RENDERER_LOG_DEBUG("Flushing dag entities");
            for (auto it = m_dagExporters.begin(), e = m_dagExporters.end(); it != e; ++it)
                it->second->flushEntities();
        }

        void exportDefaultRenderGlobals()
        {
            RENDERER_LOG_DEBUG("Exporting default render globals");

            MObject defaultRenderGlobalsNode;
            if (getDependencyNodeByName("defaultRenderGlobals", defaultRenderGlobalsNode))
            {
                // TODO: export Maya render globals if needed...
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
                    m_sessionMode,
                    *m_project,
                    m_aovs);
            }

            return appleseedRenderGlobalsNode;
        }

        void createExporters()
        {
            if (m_options.m_selectionOnly)
            {
                // Create exporters only for the selected nodes in the scene.
                MStatus status;
                MSelectionList sel;
                status = MGlobal::getActiveSelectionList(sel);

                MDagPath rootPath, path;
                MItDag it(MItDag::kBreadthFirst);

                for (int i = 0, e = sel.length(); i < e; ++i)
                {
                    status = sel.getDagPath(i, rootPath);
                    if (status)
                    {
                        for (it.reset(rootPath); !it.isDone(); it.next())
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
                for (MItDag it(MItDag::kDepthFirst); !it.isDone(); it.next())
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
            for (auto it = m_dagExporters.begin(), e = m_dagExporters.end(); it != e; ++it)
                it->second->createExporters(m_exporter_factory);

            throwIfUserAborted();

            // Create shading engine extra exporters.
            RENDERER_LOG_DEBUG("Creating shading engines extra exporters");
            for (auto it = m_shadingEngineExporters.begin(), e = m_shadingEngineExporters.end(); it != e; ++it)
                it->second->createExporters(m_exporter_factory);
        }

        void createDagNodeExporter(const MDagPath& path)
        {
            throwIfUserAborted();

            if (m_dagExporters.count(path.fullPathName()) != 0)
                return;

            MFnDagNode dagNodeFn(path);

            // Avoid warnings about missing exporter for transform nodes.
            if (dagNodeFn.typeName() == "transform")
                return;

            // Skip Maya's world node.
            if (dagNodeFn.typeName() == "dagNode" && dagNodeFn.name() == "world")
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
                    "No dag exporter found for node %s of type %s",
                    dagNodeFn.name().asChar(),
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
            std::map<MurmurHash, ShapeExporterPtr> shapesMap;

            for (auto it = m_dagExporters.begin(), e = m_dagExporters.end(); it != e; ++it)
            {
                ShapeExporter* shape = dynamic_cast<ShapeExporter*>(it->second.get());
                if (shape && shape->supportsInstancing())
                {
                    // Compute the object hash.
                    MurmurHash hash = shape->hash();
                    RENDERER_LOG_DEBUG(
                        "Computed hash for object %s, hash = %s",
                        shape->appleseedName().asChar(),
                        hash.toString().c_str());

                    // Check if we have exported this object before.
                    auto masterIt = shapesMap.find(hash);
                    if (masterIt != shapesMap.end())
                    {
                        // Create an instance exporter.
                        DagNodeExporterPtr instanceExporter(
                            new InstanceExporter(
                                shape->dagPath(),
                                m_sessionMode,
                                *masterIt->second,
                                *m_project,
                                shape->transformSequence()));

                        // Replace the shape exporter by an instance exporter.
                        m_dagExporters[it->first] = instanceExporter;
                    }
                    else
                        shapesMap[hash] = std::dynamic_pointer_cast<ShapeExporter>(it->second);
                }
            }
        }

        void initFileLogging(MObject& globals, ScopedLogTarget& logTarget) const
        {
            const MString logFilename = RenderGlobalsNode::logFilename(globals);

            if (logFilename.length() != 0)
            {
                // Create the file log target and make sure it's open.
                asf::auto_release_ptr<asf::FileLogTarget> l(asf::create_file_log_target());
                l->open(logFilename.asChar());

                if(!l->is_open())
                {
                    RENDERER_LOG_ERROR("Could not open log file %s", logFilename.asChar());
                    return;
                }

                logTarget.setLogTarget( asf::auto_release_ptr<asf::ILogTarget>(l.release()));
            }
        }

        void finalRender()
        {
            assert(MGlobal::mayaState() == MGlobal::kInteractive);
            assert(m_computation);

            // Get the appleseed globals node.
            MObject appleseedRenderGlobalsNode;
            getDependencyNodeByName("appleseedRenderGlobals", appleseedRenderGlobalsNode);
            MFnDependencyNode depNodeFn(appleseedRenderGlobalsNode);

            // Init logging.
            asr::global_logger().set_verbosity_level(
                RenderGlobalsNode::logLevel(appleseedRenderGlobalsNode));

            // Start the idle job queue for render view updates.
            IdleJobQueue::start();

            // Reset the renderer controller.
            m_rendererController.set_status(asr::IRendererController::ContinueRendering);

            // Create a tile callback to render to Maya's render view.
            m_tileCallbackFactory.reset(
                new RenderViewTileCallbackFactory(m_rendererController, m_computation));
            m_tileCallbackFactory->renderViewStart(*m_project->get_frame());

            // Create the master renderer.
            asr::Configuration* cfg = m_project->configurations().get_by_name("final");
            const asr::ParamArray& params = cfg->get_parameters();
            m_renderer.reset(
                new asr::MasterRenderer(
                    *m_project,
                    params,
                    g_resourceSearchPaths,
                    static_cast<asr::ITileCallbackFactory*>(m_tileCallbackFactory.get())));

            // Render in a thread (non blocking).
            std::thread thread(&SessionImpl::renderFunc, this);
            m_renderThread.swap(thread);
        }

        void batchRender()
        {
            // Get the appleseed globals node.
            MObject appleseedRenderGlobalsNode;
            getDependencyNodeByName("appleseedRenderGlobals", appleseedRenderGlobalsNode);
            MFnDependencyNode depNodeFn(appleseedRenderGlobalsNode);

            // Init logging.
            ScopedSetLoggerVerbosity logLevel(RenderGlobalsNode::logLevel(appleseedRenderGlobalsNode));

            ScopedLogTarget logTarget;
            initFileLogging(appleseedRenderGlobalsNode, logTarget);

            // Reset the renderer controller.
            m_rendererController.set_status(asr::IRendererController::ContinueRendering);

            // Create the master renderer.
            asr::Configuration* cfg = m_project->configurations().get_by_name("final");
            const asr::ParamArray& params = cfg->get_parameters();
            m_renderer.reset(
                new asr::MasterRenderer(
                    *m_project,
                    params,
                    g_resourceSearchPaths,
                    static_cast<asr::ITileCallbackFactory*>(nullptr)));

            // Render in the main thread (blocking).
            m_renderer->render(m_rendererController);
        }

        void progressiveRender()
        {
            /*
            // Reset the renderer controller.
            m_rendererController->set_status(asr::IRendererController::ContinueRendering);

            // Create the master renderer.
            asr::Configuration* cfg = m_project->configurations().get_by_name("interactive");
            const asr::ParamArray &params = cfg->get_parameters();

            RenderViewTileCallbackFactory tileCallbackFactory;
            m_renderer.reset( new asr::MasterRenderer(*m_project, params, m_rendererController, &tileCallbackFactory));
            */
        }

        void renderFunc()
        {
            m_renderer->render(m_rendererController);
            IdleJobQueue::pushJob(&AppleseedSession::endSession);
        }

        void abortRender()
        {
            // Ask appleseed to stop rendering.
            m_rendererController.set_status(asr::IRendererController::AbortRendering);

            // Wait for the render thread to finish.
            if (m_renderThread.joinable())
                m_renderThread.join();
        }

        asf::AABB3d computeSceneBoundingBox() const
        {
            asf::AABB3d bbox;

            // Compute the bounding box of all the objects exported.
            for (auto it = m_dagExporters.begin(), e = m_dagExporters.end(); it != e; ++it)
                bbox += it->second->boundingBox();

            // Apply the scene scale factor.
            const asr::AssemblyInstance* assemblyInstance =
                m_project->get_scene()->assembly_instances().get_by_name("assembly_inst");
            return assemblyInstance->transform_sequence().to_parent(bbox);
        }

        bool writeProject() const
        {
            return writeProject(m_fileName.asChar());
        }

        bool writeProject(const char* filename) const
        {
            if (m_options.m_writeBoundingBox)
            {
                // Save the bounding box of the scene.
                asf::AABB3d bbox = computeSceneBoundingBox();

                bfs::path path(filename);
                path.replace_extension(".bounds");

                std::ofstream ofs(path.string());
                ofs << "bounds = ["
                    << bbox.min.x << ", " << bbox.min.y << ", " << bbox.min.z << ", "
                    << bbox.max.x << ", " << bbox.max.y << ", " << bbox.max.z << "]";
            }

            const bool packed = asf::ends_with(filename, ".appleseedz");
            return asr::ProjectFileWriter::write(
                *m_project,
                filename,
                packed
                  ? asr::ProjectFileWriter::Defaults
                  : asr::ProjectFileWriter::OmitHandlingAssetFiles | asr::ProjectFileWriter::OmitWritingGeometryFiles);
        }

        void WriteImages(const char* filename) const
        {
            const asr::Frame* frame = m_project->get_frame();
            frame->write_main_image(filename);
            frame->write_aov_images(filename);
        }

        asr::Assembly* mainAssembly()
        {
            asr::Scene* scene = m_project->get_scene();
            return scene->assemblies().get_by_name("assembly");
        }

        void throwIfUserAborted() const
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
        ExporterFactory                                         m_exporter_factory;
        MTime                                                   m_savedTime;

        asf::auto_release_ptr<renderer::Project>                m_project;
        asr::AOVContainer                                       m_aovs;

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
}

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
}

MStatus projectExport(const MString& fileName, const Options& options)
{
    // In case we were doing IPR.
    endSession();

    ScopedEndSession session;
    ComputationPtr computation = Computation::create();

    g_savedTime = MAnimControl::currentTime();
    g_savedLogLevel = asr::global_logger().get_verbosity_level();

    if (options.m_sequence)
    {
        std::string fname_template = fileName.asChar();
        if (fname_template.find('#') == std::string::npos)
        {
            RENDERER_LOG_ERROR("No frame placeholders in filename.");
            return MS::kFailure;
        }

        for (int frame = options.m_firstFrame; frame <= options.m_lastFrame; frame += options.m_frameStep)
        {
            // Check if the user wants to abort the export.
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

MStatus render(const Options& options)
{
    // In case we were doing IPR.
    endSession();

    ComputationPtr computation = Computation::create();

    g_savedTime = MAnimControl::currentTime();
    g_savedLogLevel = asr::global_logger().get_verbosity_level();

    try
    {
        beginSession(FinalRenderSession, options, computation);
        g_globalSession->exportProject();

        if (computation->isInterruptRequested())
        {
            RENDERER_LOG_INFO("Render aborted.");
            return MS::kSuccess;
        }

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

    MStatus batchRenderFrame(const Options& options, const MString& outputFilename)
    {
        ScopedEndSession session;

        try
        {
            beginSession(BatchRenderSession, options, ComputationPtr());
            g_globalSession->exportProject();
            g_globalSession->batchRender();
            g_globalSession->WriteImages(outputFilename.asChar());
        }
        catch (...)
        {
            return MS::kFailure;
        }

        return MS::kSuccess;
    }
}

MStatus batchRender(Options options)
{
    // In case we were doing IPR.
    endSession();

    MStatus status;

    // HACK!: update the file format and file extension in the render globals.
    MGlobal::executePythonCommand(
        "import appleseedMaya.renderGlobals\n"
        "appleseedMaya.renderGlobals.imageFormatChanged()");

    // Get the current scene name.
    MString sceneName;
    MGlobal::executeCommand("file -q -sn -shn", sceneName);

    if (sceneName.length() != 0)
    {
        // When doing batch rendering, Maya saves the scene to a temporary file
        // /path/to/scene/name__xxxx.mb, where xxxx can be 4 or 5 digits, depending
        // on the Maya version.
        // To recover the original scene filename, we strip the __xxxx.mb suffix.
        const size_t suffixPos = std::string(sceneName.asChar()).find_last_of('_') - 1;

        assert(sceneName.length() > suffixPos);
        sceneName = MString( sceneName.asChar(), static_cast<int>(suffixPos));
    }
    else
        sceneName.set("untitled");

    // Use the first renderable camera as the render camera.
    MString cameraName;
    {
        MDagPath path;
        for (MItDag it(MItDag::kDepthFirst, MFn::kCamera); !it.isDone(); it.next())
        {
            it.getPath(path);
            MFnDagNode dagNodeFn(path);

            bool isRenderable = false;
            AttributeUtils::get(path.node(), "renderable", isRenderable);

            if (isRenderable)
            {
                options.m_camera = dagNodeFn.partialPathName();
                cameraName = dagNodeFn.name();
                break;
            }
        }
    }

    MObject renderLayer = MFnRenderLayer::currentLayer(&status);

    MCommonRenderSettingsData renderSettings;
    MRenderUtil::getCommonRenderSettings(renderSettings);

    // TODO: check if this is needed and how to get it...
    MString fileFormat;

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

namespace
{

bool doExportSwatch(
    asr::Project&               project,
    const MObject&              node,
    const MPlug&                outputPlug,
    const ShadingNetworkContext context)
{
    asr::Scene* scene = project.get_scene();
    assert(scene);

    asr::Assembly* ass = scene->assemblies().get_by_name("assembly");
    assert(ass);

    ass->shader_groups().clear();

    ShadingNetworkExporterPtr exporter(NodeExporterFactory::createShadingNetworkExporter(
        context,
        node,
        outputPlug,
        *ass,
        FinalRenderSession));

    exporter->createEntities();
    exporter->flushEntities();

    asr::Material* material = ass->materials().get_by_index(0);
    assert(material);

    material->get_parameters().insert("osl_surface", exporter->shaderGroupName().asChar());
    return true;
}

}

bool exportMaterialSwatch(asr::Project& project, const MObject& node)
{
    MFnDependencyNode depNodeFn(node);
    MPlug outputPlug = depNodeFn.findPlug("outColor", /*wantNetworkedPlug=*/ false);

    if (outputPlug.isNull())
        return false;

    return doExportSwatch(
        project,
        node,
        outputPlug,
        SurfaceSwatchNetworkContext);
}

bool exportTextureSwatch(asr::Project& project, const MObject& node)
{
    MFnDependencyNode depNodeFn(node);

    // Try outColor first.
    MPlug outputPlug = depNodeFn.findPlug("outColor", /*wantNetworkedPlug=*/ false);

    // Try outAlpha next.
    if (outputPlug.isNull())
        outputPlug = depNodeFn.findPlug("outAlpha", /*wantNetworkedPlug=*/ false);

    // Give up if we don't have a plug.
    if (outputPlug.isNull())
        return false;

    return doExportSwatch(
        project,
        node,
        outputPlug,
        TextureSwatchNetworkContext);
}

void endSession()
{
    if (g_globalSession.get())
    {
        g_globalSession.reset();

        if (g_savedTime != MAnimControl::currentTime())
            MGlobal::viewFrame(g_savedTime);

        asr::global_logger().set_verbosity_level(g_savedLogLevel);

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
