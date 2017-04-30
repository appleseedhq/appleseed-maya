
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
#include "appleseedmaya/swatchrenderer.h"

// Maya headers.
#include <maya/MFnDependencyNode.h>
#include <maya/MImage.h>

// appleseed.foundation headers.
#include "foundation/math/vector.h"
#include "foundation/utility/iostreamop.h"

// appleseed.renderer headers.
#include "renderer/api/camera.h"
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

// appleseed.maya headers.
#include "appleseedmaya/imageutils.h"
#include "appleseedmaya/logger.h"

namespace asf = foundation;
namespace asr = renderer;

namespace
{

asf::auto_release_ptr<asr::Project> g_project;
asr::Assembly*                      g_mainAssembly;
asr::Material*                      g_material;
asr::MasterRenderer*                g_renderer;
asr::DefaultRendererController      g_rendererController;

}

const MString SwatchRenderer::name("AppleseedRenderSwatch");
const MString SwatchRenderer::fullName("swatch/AppleseedRenderSwatch");

void SwatchRenderer::initialize()
{
    assert(g_project.get() == 0);

    g_project = asr::ProjectFactory::create("project");
    g_project->add_default_configurations();

    const size_t NumThreads = 2;

    // Insert some config params needed by the interactive renderer.
    asr::Configuration *cfg = g_project->configurations().get_by_name("interactive");
    asr::ParamArray *cfg_params = &cfg->get_parameters();
    cfg_params->insert("sample_renderer", "generic");
    cfg_params->insert("sample_generator", "generic");
    cfg_params->insert("tile_renderer", "generic");
    cfg_params->insert("frame_renderer", "progressive");
    cfg_params->insert("lighting_engine", "pt");
    cfg_params->insert("pixel_renderer", "uniform");
    cfg_params->insert("sampling_mode", "qmc");
    cfg_params->insert_path("progressive_frame_renderer.max_fps", "5");
    cfg_params->insert("rendering_threads", NumThreads);

    // Insert some config params needed by the final renderer.
    cfg = g_project->configurations().get_by_name("final");
    cfg_params = &cfg->get_parameters();
    cfg_params->insert("sample_renderer", "generic");
    cfg_params->insert("sample_generator", "generic");
    cfg_params->insert("tile_renderer", "generic");
    cfg_params->insert("frame_renderer", "generic");
    cfg_params->insert("lighting_engine", "pt");
    cfg_params->insert("pixel_renderer", "uniform");
    cfg_params->insert("sampling_mode", "qmc");
    cfg_params->insert_path("uniform_pixel_renderer.samples", "4");
    cfg_params->insert("rendering_threads", NumThreads);

    // Create some basic project entities.

    // Create the scene
    asf::auto_release_ptr<asr::Scene> scene = asr::SceneFactory::create();
    g_project->set_scene(scene);

    // Create the camera.
    asf::auto_release_ptr<asr::Camera> camera = asr::PinholeCameraFactory().create(
        "camera",
        asr::ParamArray()
            .insert("film_dimensions", "0.0359999 0.0359999")
            .insert("focal_length", "0.035"));
    camera->transform_sequence().set_transform(
        0.0f,
        asf::Transformd(asf::Matrix4d::make_translation(asf::Vector3d(0.0, 0.0, 2.65))));
    g_project->get_scene()->cameras().insert(camera);

    // Create the frame.
    const size_t TileSize = 32;
    asf::auto_release_ptr<asr::Frame> frame(
        asr::FrameFactory::create(
            "beauty",
            asr::ParamArray()
                .insert("resolution", "128 128")
                .insert("camera", "camera")
                .insert("pixel_format", "uint8")
                .insert("color_space", "srgb")
                .insert("tile_size", asf::Vector2i(TileSize, TileSize))));
    g_project->set_frame(frame);

    // Create the environment.
    asf::auto_release_ptr<asr::EnvironmentEDF> environmentEDF(asr::ConstantEnvironmentEDFFactory().create(
        "environmentEDF",
        asr::ParamArray().insert("radiance", "0.2")));
    g_project->get_scene()->environment_edfs().insert(environmentEDF);

    asf::auto_release_ptr<asr::EnvironmentShader> environmentShader(asr::EDFEnvironmentShaderFactory().create(
        "environmentShader",
        asr::ParamArray()
            .insert("environment_edf", "environmentEDF")
            .insert("alpha_value", "1.0")));
    g_project->get_scene()->environment_shaders().insert(environmentShader);

    asf::auto_release_ptr<asr::Environment> environment(asr::EnvironmentFactory().create(
        "environment",
        asr::ParamArray().insert("environment_shader", "environmentShader")));
    g_project->get_scene()->set_environment(environment);

    // Create the main assembly.
    asf::auto_release_ptr<asr::Assembly> assembly = asr::AssemblyFactory().create("assembly", asr::ParamArray());
    g_mainAssembly = assembly.get();
    g_project->get_scene()->assemblies().insert(assembly);

    // Create the light.
    asf::auto_release_ptr<asr::Light> light = asr::DirectionalLightFactory().create(
        "light",
        asr::ParamArray().insert("irradiance", "2.0"));
    g_mainAssembly->lights().insert(light);

    // Create the material.
    asf::auto_release_ptr<asr::Material> material = asr::OSLMaterialFactory().create(
        "material",
        asr::ParamArray());
    g_material = material.get();
    g_mainAssembly->materials().insert(material);

    // Create the geometry.
    asf::auto_release_ptr<asr::MeshObject> sphere = asr::create_primitive_mesh(
        "sphere",
        asr::ParamArray()
            .insert("primitive", "sphere")
            .insert("radius", "1.0")
            .insert("resolution_u", "30")
            .insert("resolution_v", "15"));
    g_mainAssembly->objects().insert(asf::auto_release_ptr<asr::Object>(sphere.release()));

    asf::StringDictionary materials;
    materials.insert("default", g_material->get_name());

    asf::auto_release_ptr<asr::ObjectInstance> objInstance = asr::ObjectInstanceFactory().create(
        "sphereInstance",
        asr::ParamArray(),
        "sphere",
        asf::Transformd::make_identity(),
        materials,
        materials);
    g_mainAssembly->object_instances().insert(objInstance);

    // Instance the main assembly.
    asf::auto_release_ptr<asr::AssemblyInstance> assemblyInstance = asr::AssemblyInstanceFactory::create("assembly_inst", asr::ParamArray(), "assembly");
    g_project->get_scene()->assembly_instances().insert(assemblyInstance);

    // While testing...
    cfg_params->insert_path(
        "shading_engine.override_shading.mode",
        "facing_ratio");

    // Create the master renderer.
    g_renderer =
        new asr::MasterRenderer(
            g_project.ref(),
            g_project->configurations().get_by_name("final")->get_inherited_parameters(),
            &g_rendererController);

    RENDERER_LOG_INFO("Initialized swatch renderer.");
}

void SwatchRenderer::uninitialize()
{
    delete g_renderer;
    g_project.reset();
    RENDERER_LOG_INFO("Uninitialized swatch renderer.");
}

MSwatchRenderBase* SwatchRenderer::creator(
    MObject dependNode,
    MObject renderNode,
    int     imageResolution)
{
    return new SwatchRenderer(dependNode, renderNode, imageResolution);
}

SwatchRenderer::SwatchRenderer(
    MObject dependNode,
    MObject renderNode,
    int     imageResolution)
  : MSwatchRenderBase(dependNode, renderNode, imageResolution)
{
}

bool SwatchRenderer::doIteration()
{
    MFnDependencyNode depNodeFn(node());
    const MString name = depNodeFn.name();
    const MString typeName = depNodeFn.typeName();
    const MString classification = MFnDependencyNode::classification(typeName);

    RENDERER_LOG_DEBUG(
        "Rendering swatch for node %s of type %s and classification %s. res = %d",
        name.asChar(),
        typeName.asChar(),
        classification.asChar(),
        resolution());

    // Disable logging while rendering the swatch.
    ScopedSetLoggerVerbosity logLevel(asf::LogMessage::Error);

    // Remove all shadergroups.
    g_mainAssembly->shader_groups().clear();
    g_material->get_parameters().remove_path("osl_surface");

    // todo: export node() here...

    image().create(resolution(), resolution());

    // Recreate the frame.
    asr::ParamArray frameParams = g_project->get_frame()->get_parameters();
    frameParams.insert("resolution", asf::Vector2i(resolution(), resolution()));
    asf::auto_release_ptr<asr::Frame> frame(asr::FrameFactory::create("beauty", frameParams));
    g_project->set_frame(frame);

    g_renderer->render();

    ImageUtils::copySwatchImage(g_project->get_frame()->image(), image());
    return true;
}
