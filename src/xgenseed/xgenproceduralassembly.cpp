
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2017 Esteban Tovagliari, The appleseedhq Organization
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

// appleseed.renderer headers.
#include "renderer/api/camera.h"
#include "renderer/api/frame.h"
#include "renderer/api/project.h"
#include "renderer/api/scene.h"

// appleseed.foundation headers.
#include "foundation/math/matrix.h"
#include "foundation/utility/string.h"

// XGen headers.
#include <XGen/XgRenderAPI.h>
#include <XGen/XgRenderAPIUtils.h>

// Standard headers.
#include <cstring>
#include <memory>
#include <sstream>


namespace asf = foundation;
namespace asr = renderer;
using namespace XGenRenderAPI;
using namespace XGenRenderAPI::Utils;


class XGenCallbacks
  : public ProceduralCallbacks
{
  public:
    explicit XGenCallbacks(asr::Assembly& assembly)
      : m_assembly(assembly)
      , m_params(assembly.get_parameters())
    {
    }

    void flush(const char* in_geom, PrimitiveCache* in_cache) override
    {
        if (in_cache->get(PrimitiveCache::PrimIsSpline))
        {
            flush_splines(in_geom, in_cache);
        }
        else
        {
            const char* primitive_type = in_cache->get(PrimitiveCache::PrimitiveType);

            if (strcmp(primitive_type, "CardPrimitive") == 0)
                flush_cards(in_geom, in_cache);
            else if (strcmp(primitive_type, "SpherePrimitive") == 0)
                flush_spheres(in_geom, in_cache);
            else if (strcmp(primitive_type, "ArchivePrimitive") ==0)
                flush_archives(in_geom, in_cache);
            else
            {
                RENDERER_LOG_ERROR(
                    "XGenCallbacks: unknown primitive type %s found", primitive_type);
            }
        }
    }

    void flush_splines(const char* in_geom, PrimitiveCache* in_cache)
    {
        RENDERER_LOG_DEBUG("XGenCallbacks: flush_splines called");
    }

    void flush_cards(const char* in_geom, PrimitiveCache* in_cache)
    {
        RENDERER_LOG_DEBUG("XGenCallbacks: flush_cards called");
    }

    void flush_spheres(const char* in_geom, PrimitiveCache* in_cache)
    {
        RENDERER_LOG_DEBUG("XGenCallbacks: flush_spheres called");
    }

    void flush_archives(const char* in_geom, PrimitiveCache* in_cache)
    {
        RENDERER_LOG_DEBUG("XGenCallbacks: flush_archives called");
    }

    void log(const char* in_str)  override
    {
        RENDERER_LOG_INFO("XGen procedural assembly: %s", in_str);
    }

    bool get(EBoolAttribute attr) const override
    {
        switch (attr)
        {
            case ClearDescriptionCache:
            case DontUsePaletteRefCounting:
                return false;
        }
    }

    float get(EFloatAttribute attr) const override
    {
        switch (attr)
        {
            case ShadowMotionBlur:
                return get_param("ShadowMotionBlur", 0.0f);

            case ShutterOffset:
                return get_param("ShutterOffset", 0.0f);
        }
    }

    const char* get(EStringAttribute attr) const override
    {
        switch (attr)
        {
            case BypassFXModulesAfterBGM:
                return get_string("BypassFXModulesAfterBGM");

            case CacheDir:
                return get_string("CacheDir", "xgenCache/");

            case Generator:
                return get_string("Generator", "undefined");

            case Off:
            {
                const asr::ParamArray& params = get_parameters();

                if (params.strings().exist("Off"))
                {
                    const char* str = params.get_path("Off");
                    if (stob(str))
                        return "xgen_OFF";
                }

                return "";
            }

            case Phase:
                return get_string("Phase", "color");

            case RenderCam:
                return get_string("irRenderCam");

            case RenderCamFOV:
                return get_string("irRenderCamFOV");

            case RenderCamRatio:
                return get_string("irRenderCamRatio");

            case RenderCamXform:
            {
                return get_string("irRenderCamXform");
            }

            case RenderMethod:
                return get_string("RenderMethod");
        }
    }

    const float* get(EFloatArrayAttribute attr) const override
    {
        switch (attr)
        {
            case DensityFalloff:
            case LodHi:
            case LodLow:
            case LodMed:
            case Shutter:
                return nullptr;
        }
    }

    unsigned int getSize(EFloatArrayAttribute attr) const override
    {
        switch (attr)
        {
            case DensityFalloff:
            case LodHi:
            case LodLow:
            case LodMed:
            case Shutter:
                return 0;
        }
    }

    const char* getOverride(const char* name) const override
    {
        return get_string(name);
    }

    void getTransform(float in_time, mat44& out_mat) const override
    {
        RENDERER_LOG_DEBUG("XGenCallbacks: getTransform called");
    }

    bool getArchiveBoundingBox(const char* in_filename, bbox& out_bbox) const override
    {
        RENDERER_LOG_DEBUG("XGenCallbacks: getArchiveBoundingBox called");
        return false;
    }

  private:
    asr::Assembly&          m_assembly;
    const asr::ParamArray&  m_params;

    const asr::ParamArray& get_parameters() const
    {
        return m_params;
    }

    const char* get_string(const char* key, const char* default_value = "") const
    {
        if (m_params.strings().exist(key))
            return m_params.get(key);

        return default_value;
    }

    template <typename T>
    T get_param(const char* key, const T& default_value) const
    {
        return m_params.get_optional(key, default_value);
    }
};


class XGenFaceAssembly
  : public asr::ProceduralAssembly
{
  public:
    XGenFaceAssembly(
        const char*             name,
        const asr::ParamArray   params,
        PatchRenderer*          patch_renderer,
        unsigned int            face_id)
      : asr::ProceduralAssembly(name, params)
      , m_patch_renderer(patch_renderer)
    {
    }

    void release() override
    {
        delete this;
    }

    bool expand_contents(
        const asr::Project&     project,
        const asr::Assembly*    parent,
        asf::IAbortSwitch*      abort_switch = 0) override
    {
        m_xgen_callbacks.reset(new XGenCallbacks(*this));
        std::unique_ptr<FaceRenderer> face_renderer(FaceRenderer::init(
            m_patch_renderer,
            m_face_id,
            m_xgen_callbacks.get()));
        return face_renderer->render();
    }

  private:
    std::unique_ptr<XGenCallbacks>  m_xgen_callbacks;
    PatchRenderer*                  m_patch_renderer;
    unsigned int                    m_face_id;
};


class XGenPatchAssembly
  : public asr::PluginAssembly
{
  public:
    XGenPatchAssembly(const char* name, const asr::ParamArray params)
      : asr::PluginAssembly(name, params)
    {
    }

    void release() override
    {
        delete this;
    }

    bool expand_contents(
        const asr::Project&     project,
        const asr::Assembly*    parent,
        asf::IAbortSwitch*      abort_switch = 0) override
    {
        std::string xgen_args;

        try
        {
            xgen_args = get_parameters().get("xgen_args");
        }
        catch (const asf::ExceptionDictionaryKeyNotFound&)
        {
            RENDERER_LOG_ERROR("XGen procedural error: missing xgen_args parameter");
            return false;
        }

        // Compute and save some parameters needed by XGen.
        {
            // Fetch the camera.
            const asr::Frame* frame = project.get_frame();
            const asr::Camera *camera = project.get_scene()->cameras().get_by_name(
                frame->get_active_camera_name());

            const asf::Transformd& transform =
                camera->transform_sequence().get_earliest_transform();

            bool camera_is_persp = false;
            if (strcmp(camera->get_model(), "pinhole_camera") == 0)
                camera_is_persp = true;
            else if (strcmp(camera->get_model(), "thinlens_camera") == 0)
                camera_is_persp = true;

            asr::ParamArray& params = get_parameters();

            if (!params.strings().exist("irRenderCam"))
            {
                asf::Vector3d camera_pos_or_dir;

                if (camera_is_persp)
                    camera_pos_or_dir = transform.get_local_to_parent().extract_translation();
                else
                    camera_pos_or_dir = transform.vector_to_parent(asf::Vector3d(0.0, 0.0, 1.0));

                get_parameters().insert_path(
                    "irRenderCam",
                    asf::format(
                        "{0}, {1}, {2}, {3}",
                        camera_is_persp ? "false" : "true",
                        camera_pos_or_dir.x,
                        camera_pos_or_dir.y,
                        camera_pos_or_dir.z));
            }

            if (!params.strings().exist("irRenderCamFOV"))
            {
                if (camera_is_persp)
                    params.insert_path("irRenderCamFOV", "54.0");
                else
                    params.insert_path("irRenderCamFOV", "90.0");
            }

            if (!params.strings().exist("irRenderCamRatio"))
                params.insert_path("irRenderCamRatio", "1.0");

            if (!params.strings().exist("irRenderCamXform"))
            {
                const asf::Matrix4d& m = transform.get_parent_to_local();
                std::stringstream ss;
                ss << m(0, 0) << "," << m(0, 1) << "," << m(0, 2) << "," << m(0, 3) << ",";
                ss << m(1, 0) << "," << m(1, 1) << "," << m(1, 2) << "," << m(1, 3) << ",";
                ss << m(2, 0) << "," << m(2, 1) << "," << m(2, 2) << "," << m(2, 3) << ",";
                ss << m(3, 0) << "," << m(3, 1) << "," << m(3, 2) << "," << m(3, 3);
                params.insert_path("irRenderCamXform", ss.str().c_str());
            }
        }

        m_xgen_callbacks.reset(new XGenCallbacks(*this));
        std::unique_ptr<PatchRenderer> patch_renderer(PatchRenderer::init(m_xgen_callbacks.get(), xgen_args.c_str()));

        if (!patch_renderer)
        {
            RENDERER_LOG_ERROR("Error creating XGen patch renderer");
            return false;
        }

        bbox bbox;
        unsigned int face_id;

        while (patch_renderer->nextFace(bbox, face_id))
        {
            if (isEmpty(bbox))
                continue;

            std::string face_assembly_name = asf::format("xgen_face_assembly_{0}", face_id);
            asf::auto_release_ptr<asr::Assembly> face_assembly(
                new XGenFaceAssembly(
                    face_assembly_name.c_str(),
                    get_parameters(),
                    patch_renderer.get(),
                    face_id));

            std::string face_assembly_instance_name = asf::format("xgen_face_assembly_{0}_instance", face_id);
            asf::auto_release_ptr<asr::AssemblyInstance> face_assembly_instance(
                asr::AssemblyInstanceFactory().create(
                    face_assembly_instance_name.c_str(),
                    asr::ParamArray(),
                    face_assembly_name.c_str()));

            assemblies().insert(face_assembly);
            assembly_instances().insert(face_assembly_instance);
        }

        return true;
    }

  private:
    std::unique_ptr<XGenCallbacks>  m_xgen_callbacks;
};


class XGenPatchAssemblyFactory
  : public asr::PluginAssemblyFactory
{
  public:
    // Delete this instance.
    void release() override
    {
        delete this;
    }

    // Create a new assembly.
    asf::auto_release_ptr<asr::Assembly> create(
        const char*             name,
        const asr::ParamArray&  params = asr::ParamArray()) const override
    {
        return asf::auto_release_ptr<asr::Assembly>(
            new XGenPatchAssembly(name, params));
    }
};


extern "C"
{

// Procedural assembly plugin entry point.
asr::IAssemblyFactory* create_assembly_factory()
{
    return new XGenPatchAssemblyFactory();
}

}
