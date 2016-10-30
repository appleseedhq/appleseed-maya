
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016 Haggi Krey, The appleseedhq Organization
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

// standard headers.

// boost headers.
#include "boost/filesystem/convenience.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/scoped_ptr.hpp"

// Maya headers.
#include <maya/MFnDependencyNode.h>
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MObject.h>

// appleseed.foundation headers.
#include "foundation/platform/timers.h"
#include "foundation/utility/log.h"
#include "foundation/utility/searchpaths.h"
#include "foundation/utility/stopwatch.h"
#include "foundation/utility/string.h"

// appleseed.renderer headers.
#include "renderer/api/bsdf.h"
#include "renderer/api/camera.h"
#include "renderer/api/color.h"
#include "renderer/api/display.h"
#include "renderer/api/environment.h"
#include "renderer/api/environmentedf.h"
#include "renderer/api/environmentshader.h"
#include "renderer/api/frame.h"
#include "renderer/api/light.h"
#include "renderer/api/material.h"
#include "renderer/api/object.h"
#include "renderer/api/project.h"
#include "renderer/api/rendering.h"
#include "renderer/api/scene.h"
#include "renderer/api/shadergroup.h"
#include "renderer/api/surfaceshader.h"
#include "renderer/api/texture.h"
#include "renderer/api/utility.h"


namespace bfs = boost::filesystem;
namespace asf = foundation;
namespace asr = renderer;

namespace
{

// Plugin path.
bfs::path g_pluginPath;

// Global session.
boost::scoped_ptr<AppleseedSession> g_globalSession;

} // unnamed

MStatus AppleseedSession::initialize(const MString& pluginPath)
{
    g_pluginPath = pluginPath.asChar();
    return MS::kSuccess;
}

MStatus AppleseedSession::uninitialize()
{
    g_globalSession.reset();
    return MS::kSuccess;
}

void AppleseedSession::beginProjectExport(
    const MString& fileName,
    const Options& options)
{
    assert(g_globalSession.get() == 0);

    g_globalSession.reset(new AppleseedSession(fileName));
    g_globalSession->exportScene(options);
}

void AppleseedSession::endProjectExport()
{
    assert(g_globalSession.get());
}

void AppleseedSession::beginFinalRender(
    const Options& options)
{
    assert(g_globalSession.get() == 0);
}

void AppleseedSession::endFinalRender()
{
    assert(g_globalSession.get());
}

void AppleseedSession::beginProgressiveRender(
    const Options& options)
{
    assert(g_globalSession.get() == 0);
}

void AppleseedSession::endProgressiveRender()
{
    assert(g_globalSession.get());
}

AppleseedSession::AppleseedSession()
{
    createProject();
}

AppleseedSession::AppleseedSession(const MString& fileName) : m_fileName(fileName)
{
    m_projectPath = bfs::path(fileName.asChar()).parent_path();

    // Create a dir to store the geom files if it does not exist yet.
    boost::filesystem::path geomPath = m_projectPath / "_geometry";
    if(!boost::filesystem::exists(geomPath))
    {
        if(!boost::filesystem::create_directory(geomPath))
        {
            // todo: throw something here...
        }
    }

    createProject();

    // Set the project filename and add the project directory to the search paths.
    m_project->set_path(m_fileName.asChar());
    m_project->search_paths().set_root_path(m_projectPath.string().c_str());
}

AppleseedSession::~AppleseedSession()
{
    delete m_renderer;
}

void AppleseedSession::createProject()
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
    cfg_params->insert("sampling_mode", "rng");
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
    cfg_params->insert("sampling_mode", "rng");
    cfg_params->insert_path("uniform_pixel_renderer.samples", "64");

    // Create some basic project entities.
    asf::auto_release_ptr<asr::Frame> frame(
        asr::FrameFactory::create("beauty", asr::ParamArray().insert("resolution", "640 480")));
    m_project->set_frame(frame);

    // 16 bits float (half) is the default pixel format in appleseed.
    // Force the pixel format to float to avoid half -> float conversions.
    m_project->get_frame()->get_parameters().insert("pixel_format", "float");

    // Create the scene
    asf::auto_release_ptr<asr::Scene> scene = asr::SceneFactory::create();
    m_project->set_scene(scene);

    // Create the main assembly
    asf::auto_release_ptr<asr::Assembly> assembly = asr::AssemblyFactory().create("assembly", asr::ParamArray());
    asr::Assembly *mainAssembly = assembly.get();
    m_project->get_scene()->assemblies().insert(assembly);

    // Instance the main assembly
    asf::auto_release_ptr<asr::AssemblyInstance> assemblyInstance = asr::AssemblyInstanceFactory::create("assembly_inst", asr::ParamArray(), "assembly");
    m_project->get_scene()->assembly_instances().insert(assemblyInstance);
}

void AppleseedSession::exportScene(const Options& options)
{
    exportRenderGlobals(options);
}

void AppleseedSession::exportRenderGlobals(const Options& options)
{
    MStatus status;

    MSelectionList selList;
    selList.add("defaultRenderGlobals");
    MObject defaultRenderGlobalsNode;
    MFnDependencyNode defaultGlobalsDepFn;
    if(!selList.isEmpty())
    {
        selList.getDependNode(0, defaultRenderGlobalsNode);
        defaultGlobalsDepFn.setObject(defaultRenderGlobalsNode);
    }

    selList.clear();
    selList.add("appleseedRenderGlobals");
    MObject appleseedRenderGlobalsNode;
    MFnDependencyNode appleseedGlobalsDepFn;
    if(!selList.isEmpty())
    {
        selList.getDependNode(0, appleseedRenderGlobalsNode);
        appleseedGlobalsDepFn.setObject(appleseedRenderGlobalsNode);
    }

    // ...
}

bool AppleseedSession::writeProject() const
{
    return writeProject(m_fileName.asChar());
}

bool AppleseedSession::writeProject(const char* fileName) const
{
    return asr::ProjectFileWriter::write(
        *m_project,
        fileName,
        asr::ProjectFileWriter::OmitHandlingAssetFiles |
        asr::ProjectFileWriter::OmitWritingGeometryFiles);
}
