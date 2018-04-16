
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
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
#include "renderglobalsnode.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/config.h"
#include "appleseedmaya/typeids.h"

// appleseed.renderer headers.
#include "renderer/api/frame.h"
#include "renderer/api/project.h"
#include "renderer/api/surfaceshader.h"
#include "renderer/api/utility.h"

// appleseed.foundation headers.
#include "foundation/math/scalar.h"
#include "foundation/utility/api/specializedapiarrays.h"
#include "foundation/utility/iostreamop.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MAnimControl.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include "appleseedmaya/_endmayaheaders.h"

namespace asf = foundation;
namespace asr = renderer;

const MString RenderGlobalsNode::nodeName("appleseedRenderGlobals");
const MTypeId RenderGlobalsNode::id(RenderGlobalsNodeTypeId);

MObject RenderGlobalsNode::m_pixelSamples;
MObject RenderGlobalsNode::m_passes;
MObject RenderGlobalsNode::m_tileSize;

MObject RenderGlobalsNode::m_sceneScale;

MObject RenderGlobalsNode::m_lightingEngine;

MObject RenderGlobalsNode::m_diagnosticShader;
MStringArray RenderGlobalsNode::m_diagnosticShaderKeys;

MObject RenderGlobalsNode::m_limitBounces;
MObject RenderGlobalsNode::m_globalBounces;
MObject RenderGlobalsNode::m_specularBounces;
MObject RenderGlobalsNode::m_glossyBounces;
MObject RenderGlobalsNode::m_diffuseBounces;
MObject RenderGlobalsNode::m_lightSamples;
MObject RenderGlobalsNode::m_envSamples;
MObject RenderGlobalsNode::m_caustics;
MObject RenderGlobalsNode::m_maxRayIntensity;

MObject RenderGlobalsNode::m_backgroundEmitsLight;
MObject RenderGlobalsNode::m_envLightNode;

MObject RenderGlobalsNode::m_motionBlur;
MObject RenderGlobalsNode::m_mbCameraSamples;
MObject RenderGlobalsNode::m_mbTransformSamples;
MObject RenderGlobalsNode::m_mbDeformSamples;
MObject RenderGlobalsNode::m_shutterOpen;
MObject RenderGlobalsNode::m_shutterClose;

MObject RenderGlobalsNode::m_renderingThreads;
MObject RenderGlobalsNode::m_maxTextureCacheSize;

MObject RenderGlobalsNode::m_denoiserMode;
MStringArray RenderGlobalsNode::m_denoiserModeKeys;

MObject RenderGlobalsNode::m_prefilterSpikes;
MObject RenderGlobalsNode::m_spikeThreshold;
MObject RenderGlobalsNode::m_patchDistanceThreshold;
MObject RenderGlobalsNode::m_denoiseScales;

MObject RenderGlobalsNode::m_imageFormat;

void* RenderGlobalsNode::creator()
{
    return new RenderGlobalsNode();
}

MStatus RenderGlobalsNode::initialize()
{
    #define CHECKED_ADD_ATTRIBUTE(attr, name)                                       \
        APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(                                       \
            status,                                                                 \
            "appleseedMaya: Failed to create render globals " #name " attribute");  \
        status = addAttribute(attr);                                                \
        APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(                                       \
            status,                                                                 \
            "appleseedMaya: Failed to add render globals " #name " attribute");

    MFnNumericAttribute numAttrFn;
    MFnEnumAttribute enumAttrFn;
    MFnMessageAttribute msgAttrFn;

    MStatus status;

    // Pixel Samples.
    m_pixelSamples = numAttrFn.create("samples", "samples", MFnNumericData::kInt, 16, &status);
    numAttrFn.setMin(1);
    CHECKED_ADD_ATTRIBUTE(m_pixelSamples, "samples")

    // Render Passes.
    m_passes = numAttrFn.create("passes", "passes", MFnNumericData::kInt, 1, &status);
    numAttrFn.setMin(1);
    CHECKED_ADD_ATTRIBUTE(m_passes, "passes")

    // Tile Size.
    m_tileSize = numAttrFn.create("tileSize", "tileSize", MFnNumericData::kInt, 64, &status);
    numAttrFn.setMin(1);
    CHECKED_ADD_ATTRIBUTE(m_tileSize, "tileSize")

    // Scene Scale.
    m_sceneScale = numAttrFn.create("sceneScale", "sceneScale", MFnNumericData::kFloat, 1.0f, &status);
    numAttrFn.setMin(0.0000001);
    CHECKED_ADD_ATTRIBUTE(m_sceneScale, "sceneScale")

    // Lighting engine.
    m_lightingEngine = enumAttrFn.create("lightingEngine", "lightingEngine", 0, &status);
    enumAttrFn.addField("Path Tracing", 0);
    CHECKED_ADD_ATTRIBUTE(m_lightingEngine, "lightingEngine")

    // Diagnostic shader override.
    m_diagnosticShader = enumAttrFn.create("diagnostics", "diagnostics", 0, &status);
    {
        short menuIndex = 0;
        enumAttrFn.addField("No Override", menuIndex++);
        m_diagnosticShaderKeys.append("no_override");

        asr::SurfaceShaderFactoryRegistrar factoryRegistrar;
        const asr::ISurfaceShaderFactory* factory = factoryRegistrar.lookup("diagnostic_surface_shader");
        assert(factory);
        asf::DictionaryArray metadata = factory->get_input_metadata();
        const asf::Dictionary& items = metadata[0].dictionary("items");

        for (auto it(items.strings().begin()), e(items.strings().end()); it != e; ++it)
        {
            enumAttrFn.addField(it.key(), menuIndex++);
            m_diagnosticShaderKeys.append(MString(it.value()));
        }
    }
    CHECKED_ADD_ATTRIBUTE(m_diagnosticShader, "diagnosticShader")

    // Limit bounces.
    m_limitBounces = numAttrFn.create("limitBounces", "limitBounces", MFnNumericData::kBoolean, true, &status);
    CHECKED_ADD_ATTRIBUTE(m_limitBounces, "limitBounces")

    // Global Bounces.
    m_globalBounces = numAttrFn.create("bounces", "bounces", MFnNumericData::kInt, 8, &status);
    numAttrFn.setMin(0);
    CHECKED_ADD_ATTRIBUTE(m_globalBounces, "bounces")

    // Specular Bounces.
    m_specularBounces = numAttrFn.create("specularBounces", "specularBounces", MFnNumericData::kInt, 8, &status);
    numAttrFn.setMin(0);
    CHECKED_ADD_ATTRIBUTE(m_specularBounces, "specularBounces")

    // Glossy Bounces.
    m_glossyBounces = numAttrFn.create("glossyBounces", "glossyBounces", MFnNumericData::kInt, 8, &status);
    numAttrFn.setMin(0);
    CHECKED_ADD_ATTRIBUTE(m_glossyBounces, "glossyBounces")

    // Diffuse Bounces.
    m_diffuseBounces = numAttrFn.create("diffuseBounces", "diffuseBounces", MFnNumericData::kInt, 3, &status);
    numAttrFn.setMin(0);
    CHECKED_ADD_ATTRIBUTE(m_diffuseBounces, "diffuseBounces")

    // Light Samples.
    m_lightSamples = numAttrFn.create("lightSamples", "lightSamples", MFnNumericData::kFloat, 1.0f, &status);
    numAttrFn.setMin(1.0);
    CHECKED_ADD_ATTRIBUTE(m_lightSamples, "lightSamples")

    // Environment Samples.
    m_envSamples = numAttrFn.create("envSamples", "envSamples", MFnNumericData::kFloat, 1.0f, &status);
    numAttrFn.setMin(1.0);
    CHECKED_ADD_ATTRIBUTE(m_envSamples, "envSamples")

    // Caustics.
    m_caustics = numAttrFn.create("caustics", "caustics", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_caustics, "caustics")

    // Max Ray Intensity.
    m_maxRayIntensity = numAttrFn.create("maxRayIntensity", "maxRayIntensity", MFnNumericData::kFloat, 0.0f, &status);
    numAttrFn.setMin(0.0);
    CHECKED_ADD_ATTRIBUTE(m_maxRayIntensity, "maxRayIntensity")

    // Background emits light.
    m_backgroundEmitsLight = numAttrFn.create("bgLight", "bgLight", MFnNumericData::kBoolean, true, &status);
    CHECKED_ADD_ATTRIBUTE(m_backgroundEmitsLight, "bgLight")

    // Motion blur enable.
    m_motionBlur = numAttrFn.create("motionBlur", "motionBlur", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_motionBlur, "motionBlur")

    m_mbCameraSamples = numAttrFn.create("mbCameraSamples", "mbCameraSamples", MFnNumericData::kInt, 2, &status);
    numAttrFn.setMin(1);
    CHECKED_ADD_ATTRIBUTE(m_mbCameraSamples, "cameraSamples")

    m_mbTransformSamples = numAttrFn.create("mbTransformSamples", "mbTransformSamples", MFnNumericData::kInt, 2, &status);
    numAttrFn.setMin(1);
    CHECKED_ADD_ATTRIBUTE(m_mbTransformSamples, "xformSamples")

    m_mbDeformSamples = numAttrFn.create("mbDeformSamples", "mbDeformSamples", MFnNumericData::kInt, 2, &status);
    numAttrFn.setMin(1);
    CHECKED_ADD_ATTRIBUTE(m_mbDeformSamples, "deformSamples")

    // Shutter Open.
    m_shutterOpen = numAttrFn.create("shutterOpen", "shutterOpen", MFnNumericData::kFloat, -0.25f, &status);
    CHECKED_ADD_ATTRIBUTE(m_shutterOpen, "shutterOpen")

    // Shutter Close.
    m_shutterClose = numAttrFn.create("shutterClose", "shutterClose", MFnNumericData::kFloat, 0.25f, &status);
    CHECKED_ADD_ATTRIBUTE(m_shutterClose, "shutterClose")

    // Rendering threads.
    m_renderingThreads = numAttrFn.create("threads", "threads", MFnNumericData::kInt, -1, &status);
    CHECKED_ADD_ATTRIBUTE(m_renderingThreads, "threads")

    // Texture cache size.
    m_maxTextureCacheSize = numAttrFn.create("maxTexCacheSize", "maxTexCacheSize", MFnNumericData::kInt, 1024, &status);
    numAttrFn.setMin(16);
    CHECKED_ADD_ATTRIBUTE(m_maxTextureCacheSize, "maxTexCacheSize")

    // Environment light connection.
    m_envLightNode = msgAttrFn.create("envLight", "env", &status);
    CHECKED_ADD_ATTRIBUTE(m_envLightNode, "envLight")

    // Denoiser.
    m_denoiserModeKeys.append("off");
    m_denoiserModeKeys.append("on");
    m_denoiserModeKeys.append("write_outputs");

    m_denoiserMode = enumAttrFn.create("denoiser", "denoiser");

    enumAttrFn.addField("Off", 0);
    enumAttrFn.addField("On", 1);
    enumAttrFn.addField("Write Outputs", 2);

    CHECKED_ADD_ATTRIBUTE(m_denoiserMode, "denoiser")

    // Prefilter Spikes.
    m_prefilterSpikes = numAttrFn.create("prefilterSpikes", "prefilterSpikes", MFnNumericData::kBoolean, true, &status);
    CHECKED_ADD_ATTRIBUTE(m_prefilterSpikes, "prefilterSpikes")

    // Spike Thereshold.
    m_spikeThreshold = numAttrFn.create("spikeThreshold", "spikeThreshold", MFnNumericData::kFloat, 2.0f, &status);
    numAttrFn.setMin(0.1);
    numAttrFn.setMax(4.0);
    CHECKED_ADD_ATTRIBUTE(m_spikeThreshold, "spikeThreshold")

    // Patch Distance.
    m_patchDistanceThreshold = numAttrFn.create("patchDistance", "patchDistance", MFnNumericData::kFloat, 1.0f, &status);
    numAttrFn.setMin(0.5);
    numAttrFn.setMax(3.0);
    CHECKED_ADD_ATTRIBUTE(m_patchDistanceThreshold, "patchDistance")

    // Denoise Scales.
    m_denoiseScales = numAttrFn.create("denoiseScales", "denoiseScales", MFnNumericData::kInt, 3, &status);
    numAttrFn.setMin(1);
    CHECKED_ADD_ATTRIBUTE(m_denoiseScales, "denoiseScales")

    // Image Format
    m_imageFormat = numAttrFn.create("imageFormat", "imageFormat", MFnNumericData::kInt, 0, &status);
    CHECKED_ADD_ATTRIBUTE(m_imageFormat, "imageFormat")

    return status;

    #undef CHECKED_ADD_ATTRIBUTE
}

MStatus RenderGlobalsNode::compute(const MPlug& plug, MDataBlock& dataBlock)
{
    return MS::kSuccess;
}

void RenderGlobalsNode::applyGlobalsToProject(
    const MObject&                              globals,
    AppleseedSession::SessionMode               sessionMode,
    asr::Project&                               project)
{
    asr::ParamArray& finalParams = project.configurations().get_by_name("final")->get_parameters();
    asr::ParamArray& iprParams   = project.configurations().get_by_name("interactive")->get_parameters();

    #define INSERT_PATH_IN_CONFIGS(path, value)     \
        {                                           \
            finalParams.insert_path(path, value);   \
            iprParams.insert_path(path, value);     \
        }

    #define REMOVE_PATH_IN_CONFIGS(path)    \
        {                                   \
            finalParams.remove_path(path);  \
            iprParams.remove_path(path);    \
        }

    int samples;
    if (AttributeUtils::get(MPlug(globals, m_pixelSamples), samples))
    {
        finalParams.insert_path("uniform_pixel_renderer.samples", samples);

        if (samples == 1)
            finalParams.insert_path("uniform_pixel_renderer.force_antialiasing", true);
    }

    int passes;
    if (AttributeUtils::get(MPlug(globals, m_passes), passes))
    {
        finalParams.insert_path("generic_frame_renderer.passes", passes);
        finalParams.insert_path("shading_result_framebuffer", passes == 1 ? "ephemeral" : "permanent");
    }

    asr::Frame* frame = project.get_frame();

    int tileSize;
    if (AttributeUtils::get(MPlug(globals, m_tileSize), tileSize))
    {
        frame->get_parameters().insert(
            "tile_size", asf::Vector2i(tileSize));
    }

    int diagnostic;
    if (AttributeUtils::get(MPlug(globals, m_diagnosticShader), diagnostic))
    {
        if (diagnostic != 0)
        {
            INSERT_PATH_IN_CONFIGS(
                "shading_engine.override_shading.mode",
                m_diagnosticShaderKeys[diagnostic])
        }
    }

    int lightingEngine;
    if (AttributeUtils::get(MPlug(globals, m_lightingEngine), lightingEngine))
    {
        if (lightingEngine == 0)
            finalParams.insert_path("lighting_engine", "pt");
    }

    // Path tracing params.
    bool limitBounces = false;
    AttributeUtils::get(MPlug(globals, m_limitBounces), limitBounces);

    if (limitBounces)
    {
        int bounces = -1;
        if (AttributeUtils::get(MPlug(globals, m_globalBounces), bounces))
            INSERT_PATH_IN_CONFIGS("pt.max_bounces", bounces)

        bounces = -1;
        if (AttributeUtils::get(MPlug(globals, m_specularBounces), bounces))
            INSERT_PATH_IN_CONFIGS("pt.max_specular_bounces", bounces)

        bounces = -1;
        if (AttributeUtils::get(MPlug(globals, m_glossyBounces), bounces))
            INSERT_PATH_IN_CONFIGS("pt.max_glossy_bounces", bounces)

        bounces = -1;
        if (AttributeUtils::get(MPlug(globals, m_diffuseBounces), bounces))
            INSERT_PATH_IN_CONFIGS("pt.max_diffuse_bounces", bounces)
    }

    bool caustics;
    if (AttributeUtils::get(MPlug(globals, m_caustics), caustics))
        INSERT_PATH_IN_CONFIGS("pt.enable_caustics", caustics)

    float maxRayIntensity;
    if (AttributeUtils::get(MPlug(globals, m_maxRayIntensity), maxRayIntensity))
    {
        if (maxRayIntensity == 0.0f)
            REMOVE_PATH_IN_CONFIGS("pt.max_ray_intensity")
        else
            INSERT_PATH_IN_CONFIGS("pt.max_ray_intensity", maxRayIntensity)
    }

    float lightSamples;
    if (AttributeUtils::get(MPlug(globals, m_lightSamples), lightSamples))
        INSERT_PATH_IN_CONFIGS("pt.dl_light_samples", lightSamples)

    float envSamples;
    if (AttributeUtils::get(MPlug(globals, m_envSamples), envSamples))
        INSERT_PATH_IN_CONFIGS("pt.ibl_env_samples", envSamples)

    // Only save rendering threads for interactive renders.
    if (sessionMode == AppleseedSession::FinalRenderSession ||
        sessionMode == AppleseedSession::ProgressiveRenderSession)
    {
        int threads;
        if (AttributeUtils::get(MPlug(globals, m_renderingThreads), threads))
        {
            if (threads == 0)
                INSERT_PATH_IN_CONFIGS("rendering_threads", "auto")
            else
                INSERT_PATH_IN_CONFIGS("rendering_threads", threads)
        }
    }

    int maxTexCacheSize;
    if (AttributeUtils::get(MPlug(globals, m_maxTextureCacheSize), maxTexCacheSize))
    {
        uint64_t texCacheSizeinBytes = maxTexCacheSize;
        texCacheSizeinBytes *= 1024 * 1024;
        INSERT_PATH_IN_CONFIGS("texture_store.max_size", texCacheSizeinBytes);
    }

    int denoiserMode;
    if (AttributeUtils::get(MPlug(globals, m_denoiserMode), denoiserMode))
    {
        frame->get_parameters().insert(
            "denoiser", m_denoiserModeKeys[denoiserMode].asChar());
    }

    bool prefilterSpikes;
    if (AttributeUtils::get(MPlug(globals, m_prefilterSpikes), prefilterSpikes))
    {
        frame->get_parameters().insert(
            "prefilter_spikes", prefilterSpikes);
    }

    float spikeThreshold;
    if (AttributeUtils::get(MPlug(globals, m_spikeThreshold), spikeThreshold))
    {
        frame->get_parameters().insert(
            "spike_threshold", spikeThreshold);
    }

    float patchDistanceThreshold;
    if (AttributeUtils::get(MPlug(globals, m_patchDistanceThreshold), patchDistanceThreshold))
    {
        frame->get_parameters().insert(
            "patch_distance_threshold", patchDistanceThreshold);
    }

    int denoiseScales;
    if (AttributeUtils::get(MPlug(globals, m_denoiseScales), denoiseScales))
    {
        frame->get_parameters().insert(
            "denoise_scales", denoiseScales);
    }

    #undef INSERT_PATH_IN_CONFIGS
    #undef REMOVE_PATH_IN_CONFIGS
}

void RenderGlobalsNode::collectMotionBlurSampleTimes(
    const MObject&                              globals,
    AppleseedSession::MotionBlurSampleTimes&    motionBlurSampleTimes)
{
    bool enableMotionBlur = false;
    AttributeUtils::get(MPlug(globals, m_motionBlur), enableMotionBlur);

    float shutterOpenTime = 0.0f;
    AttributeUtils::get(MPlug(globals, m_shutterOpen), shutterOpenTime);

    float shutterCloseTime = 0.0f;
    AttributeUtils::get(MPlug(globals, m_shutterClose), shutterCloseTime);

    if (shutterOpenTime >= shutterCloseTime)
        enableMotionBlur = false;

    if (enableMotionBlur)
    {
        const float now = static_cast<float>(MAnimControl::currentTime().value());

        motionBlurSampleTimes.clear();
        motionBlurSampleTimes.m_shutterOpenTime = now + shutterOpenTime;
        motionBlurSampleTimes.m_shutterCloseTime = now + shutterCloseTime;

        int cameraSamples = 1;
        if (AttributeUtils::get(MPlug(globals, m_mbCameraSamples), cameraSamples))
        {
            motionBlurSampleTimes.initializeFrameSet(
                cameraSamples,
                motionBlurSampleTimes.m_shutterOpenTime,
                motionBlurSampleTimes.m_shutterCloseTime,
                motionBlurSampleTimes.m_cameraTimes);
        }

        int xformSamples = 1;
        if (AttributeUtils::get(MPlug(globals, m_mbTransformSamples), xformSamples))
        {
            motionBlurSampleTimes.initializeFrameSet(
                xformSamples,
                motionBlurSampleTimes.m_shutterOpenTime,
                motionBlurSampleTimes.m_shutterCloseTime,
                motionBlurSampleTimes.m_transformTimes);
        }

        int deformSamples = 1;
        if (AttributeUtils::get(MPlug(globals, m_mbDeformSamples), deformSamples))
        {
            if (!asf::is_pow2(deformSamples))
                deformSamples = asf::next_pow2(deformSamples);

            motionBlurSampleTimes.initializeFrameSet(
                deformSamples,
                motionBlurSampleTimes.m_shutterOpenTime,
                motionBlurSampleTimes.m_shutterCloseTime,
                motionBlurSampleTimes.m_deformTimes);
        }

        motionBlurSampleTimes.mergeTimes();
    }
    else
        motionBlurSampleTimes.initializeToCurrentFrame();
}
