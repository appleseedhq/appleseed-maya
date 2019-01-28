
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016-2018 Esteban Tovagliari, The appleseedhq Organization
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
#include "swatchrenderer.h"

// appleseed-maya headers.
#include "appleseedmaya/appleseedsession.h"
#include "appleseedmaya/logger.h"
#include "appleseedmaya/utils.h"

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

// appleseed.foundation headers.
#include "foundation/core/concepts/noncopyable.h"
#include "foundation/image/colorspace.h"
#include "foundation/image/image.h"
#include "foundation/image/tile.h"
#include "foundation/math/vector.h"
#include "foundation/math/matrix.h"
#include "foundation/math/transform.h"
#include "foundation/utility/iostreamop.h"
#include "foundation/utility/string.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MFnDependencyNode.h>
#include <maya/MImage.h>
#include <maya/MPlug.h>
#include "appleseedmaya/_endmayaheaders.h"

// Standard headers.
#include <cstring>

namespace asf = foundation;
namespace asr = renderer;

namespace
{
    class SwatchProject
      : public asf::NonCopyable
    {
      public:
        SwatchProject()
        {
        }

        void initialize(const asf::SearchPaths& resourceSearchPaths)
        {
            assert(m_project.get() == nullptr);

            // Create an appleseed project.
            m_project = asr::ProjectFactory::create("project");
            m_project->add_default_configurations();

            const size_t NumThreads = 2;

            // Insert some config params needed by the final renderer.
            asr::Configuration* cfg = m_project->configurations().get_by_name("final");
            asr::ParamArray* cfg_params = &cfg->get_parameters();
            cfg_params->insert("spectrum_mode", "rgb");
            cfg_params->insert("sample_renderer", "generic");
            cfg_params->insert("sample_generator", "generic");
            cfg_params->insert("tile_renderer", "generic");
            cfg_params->insert("frame_renderer", "generic");
            cfg_params->insert("lighting_engine", "pt");
            cfg_params->insert("pixel_renderer", "uniform");
            cfg_params->insert("sampling_mode", "qmc");
            cfg_params->insert("rendering_threads", NumThreads);
            cfg_params->insert_path("uniform_pixel_renderer.samples", "4");

            // Create some basic project entities.

            // Create the scene.
            asf::auto_release_ptr<asr::Scene> scene = asr::SceneFactory::create();
            m_project->set_scene(scene);

            // Create the camera.
            asf::auto_release_ptr<asr::Camera> camera = asr::PinholeCameraFactory().create(
                "camera",
                asr::ParamArray()
                    .insert("film_dimensions", "0.0359999 0.0359999")
                    .insert("focal_length", "0.035"));
            camera->transform_sequence().set_transform(
                0.0f,
                asf::Transformd(asf::Matrix4d::make_translation(asf::Vector3d(0.0, 0.0, 2.65))));
            m_project->get_scene()->cameras().insert(camera);

            // Create the environment.
            asf::auto_release_ptr<asr::EnvironmentEDF> environmentEDF(asr::ConstantEnvironmentEDFFactory().create(
                "environmentEDF",
                asr::ParamArray().insert("radiance", "0.025")));
            m_project->get_scene()->environment_edfs().insert(environmentEDF);

            asf::auto_release_ptr<asr::EnvironmentShader> environmentShader(asr::EDFEnvironmentShaderFactory().create(
                "environmentShader",
                asr::ParamArray()
                    .insert("environment_edf", "environmentEDF")
                    .insert("alpha_value", "1.0")));
            m_project->get_scene()->environment_shaders().insert(environmentShader);

            asf::auto_release_ptr<asr::Environment> environment = asr::EnvironmentFactory().create(
                "environment",
                asr::ParamArray().insert("environment_shader", "environmentShader"));
            m_project->get_scene()->set_environment(environment);

            // Create the frame.
            const size_t TileSize = 32;
            asf::auto_release_ptr<asr::Frame> frame(
                asr::FrameFactory::create(
                    "beauty",
                    asr::ParamArray()
                        .insert("resolution", "128 128")
                        .insert("camera", "camera")
                        .insert("tile_size", asf::Vector2i(TileSize, TileSize))));
            m_project->set_frame(frame);

            // Create the main assembly.
            asf::auto_release_ptr<asr::Assembly> assembly = asr::AssemblyFactory().create("assembly", asr::ParamArray());
            m_mainAssembly = assembly.get();
            m_project->get_scene()->assemblies().insert(assembly);

            // Create the material.
            asf::auto_release_ptr<asr::Material> material = asr::OSLMaterialFactory().create(
                "material",
                asr::ParamArray());
            m_material = material.get();
            m_mainAssembly->materials().insert(material);

            // Instance the main assembly.
            asf::auto_release_ptr<asr::AssemblyInstance> assemblyInstance = asr::AssemblyInstanceFactory::create(
                "assembly_inst",
                asr::ParamArray(),
                "assembly");
            m_project->get_scene()->assembly_instances().insert(assemblyInstance);

            // Create the master renderer.
            m_renderer =
                new asr::MasterRenderer(
                    m_project.ref(),
                    m_project->configurations().get_by_name("final")->get_inherited_parameters(),
                    resourceSearchPaths,
                    &m_rendererController);
        }

        void uninitialize()
        {
            m_project.reset();
            delete m_renderer;
        }

        asr::Project& getProject()
        {
            return *m_project;
        }

        void createMaterialSceneGeometry()
        {
            // Create the light.
            asf::auto_release_ptr<asr::Light> light = asr::DirectionalLightFactory().create(
                "light",
                asr::ParamArray().insert("irradiance", "2.0"));
            m_mainAssembly->lights().insert(light);

            // Create the geometry.
            asf::auto_release_ptr<asr::MeshObject> sphere = asr::create_primitive_mesh(
                "sphere",
                asr::ParamArray()
                    .insert("primitive", "sphere")
                    .insert("radius", "1.0")
                    .insert("resolution_u", "30")
                    .insert("resolution_v", "15"));
            m_mainAssembly->objects().insert(asf::auto_release_ptr<asr::Object>(sphere.release()));

            asf::StringDictionary materials;
            materials.insert("default", m_material->get_name());

            asf::auto_release_ptr<asr::ObjectInstance> objInstance = asr::ObjectInstanceFactory().create(
                "sphereInstance",
                asr::ParamArray(),
                "sphere",
                asf::Transformd::make_identity(),
                materials,
                materials);
            m_mainAssembly->object_instances().insert(objInstance);
        }

        void createTextureSceneGeometry()
        {
            // Create the geometry.
            const float size = 2.5f;
            asr::ParamArray params;
            params.insert("primitive", "grid");
            params.insert("resolution_u", 1);
            params.insert("resolution_v", 1);
            params.insert("width", size);
            params.insert("height", size);

            asf::auto_release_ptr<asr::MeshObject> plane = asr::create_primitive_mesh(
                "plane",
                params);
            m_mainAssembly->objects().insert(
                asf::auto_release_ptr<asr::Object>(plane.release()));

            asf::StringDictionary materials;
            materials.insert("default", m_material->get_name());

            // Rotate x 90 degrees.
            asf::Matrix4d m = asf::Matrix4d::make_rotation_x(asf::deg_to_rad(90.0));

            asf::auto_release_ptr<asr::ObjectInstance> objInstance = asr::ObjectInstanceFactory().create(
                "planeInstance",
                asr::ParamArray(),
                "plane",
                asf::Transformd(m),
                materials,
                asf::StringDictionary());
            m_mainAssembly->object_instances().insert(objInstance);
        }

        void render(const size_t resolution, MImage& dstImage)
        {
            // Disable logging while rendering the swatch.
            ScopedSetLoggerVerbosity logLevel(asf::LogMessage::Error);

            // Recreate the frame.
            asr::ParamArray frameParams = m_project->get_frame()->get_parameters();
            frameParams.insert("resolution", asf::Vector2u(resolution, resolution));
            asf::auto_release_ptr<asr::Frame> frame(asr::FrameFactory::create("beauty", frameParams));
            m_project->set_frame(frame);

            // Render.
            m_renderer->render();
            copySwatchImage(dstImage);
        }

      private:
        static uint8_t to_uint8(const float c)
        {
            return static_cast<uint8_t>(asf::saturate(c) * 255.0f);
        }

        void copySwatchImage(MImage& dstImage) const
        {
            const asf::Image& srcImage = m_project->get_frame()->image();
            const asf::CanvasProperties& props = srcImage.properties();
            size_t width = props.m_canvas_width;

            for (size_t ty = 0; ty < props.m_tile_count_y; ++ty)
            {
                for (size_t tx = 0; tx < props.m_tile_count_x; ++tx)
                {
                    // todo: do we need to flip the image vertically
                    // like in the render view?
                    const size_t x0 = props.m_tile_width * tx;
                    const size_t y0 = props.m_tile_height * ty;

                    const asf::Tile& tile = srcImage.tile(tx, ty);
                    const float* src = reinterpret_cast<const float*>(tile.get_storage());

                    for (size_t j = 0, je = tile.get_height(); j < je; ++j)
                    {
                        // For swatches, we assume 4 8 bit channels.
                        const size_t y = y0 + j;
                        uint8_t* dst = dstImage.pixels() + (y * width * 4) + (x0 * 4);

                        for (size_t i = 0, ie = tile.get_width(); i < ie; ++i)
                        {
                            // Maya docs say RGBA, but it is actually BGRA.
                            *dst++ = to_uint8(src[2]);
                            *dst++ = to_uint8(src[1]);
                            *dst++ = to_uint8(src[0]);
                            *dst++ = 255;
                            src += 4;
                        }
                    }
                }
            }
        }

        asf::auto_release_ptr<asr::Project> m_project;
        asr::Assembly*                      m_mainAssembly;
        asr::Material*                      m_material;
        asr::MasterRenderer*                m_renderer;
        asr::DefaultRendererController      m_rendererController;
    };

    SwatchProject       g_materialSwatchProject;
    SwatchProject       g_textureSwatchProject;
    asf::SearchPaths    g_resourceSearchPaths;
}

const MString SwatchRenderer::name("AppleseedRenderSwatch");
const MString SwatchRenderer::fullName("swatch/AppleseedRenderSwatch");

void SwatchRenderer::initialize(const MString& /*pluginPath*/)
{
    {
        // Disable logging from appleseed.
        ScopedSetLoggerVerbosity logLevel(asf::LogMessage::Error);

        g_materialSwatchProject.initialize(g_resourceSearchPaths);
        g_materialSwatchProject.createMaterialSceneGeometry();

        g_textureSwatchProject.initialize(g_resourceSearchPaths);
        g_textureSwatchProject.createTextureSceneGeometry();
    }

    RENDERER_LOG_INFO("Initialized swatch renderer.");
}

void SwatchRenderer::uninitialize()
{
    {
        // Disable logging from appleseed.
        ScopedSetLoggerVerbosity logLevel(asf::LogMessage::Error);

        g_materialSwatchProject.uninitialize();
        g_textureSwatchProject.uninitialize();
    }

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

    /*
    RENDERER_LOG_DEBUG(
        "Rendering swatch for node %s of type %s and classification %s. res = %d",
        name.asChar(),
        typeName.asChar(),
        classification.asChar(),
        resolution());
    */

    // Allocate the pixels.
    image().create(resolution(), resolution());

    if (strstr(classification.asChar(), "rendernode/appleseed/surface") != nullptr)
    {
        if (AppleseedSession::exportMaterialSwatch(g_materialSwatchProject.getProject(), node()))
        {
            g_materialSwatchProject.render(resolution(), image());
            return true;
        }
    }
    else if (strstr(classification.asChar(), "rendernode/appleseed/texture") != nullptr)
    {
        AppleseedSession::exportTextureSwatch(g_textureSwatchProject.getProject(), node());
        {
            g_textureSwatchProject.render(resolution(), image());
            return true;
        }
    }

    return false;
}
