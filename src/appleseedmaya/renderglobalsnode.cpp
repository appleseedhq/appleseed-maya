
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
#include "appleseedmaya/renderglobalsnode.h"

// Maya headers.
#include <maya/MAnimControl.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include "appleseedmaya/_endmayaheaders.h"

// appleseed.foundation headers.
#include "foundation/utility/api/specializedapiarrays.h"
#include "foundation/math/scalar.h"

// appleseed.renderer headers.
#include "renderer/api/project.h"
#include "renderer/api/utility.h"
#include "renderer/api/surfaceshader.h"

// appleseed.maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/config.h"
#include "appleseedmaya/typeids.h"

namespace asf = foundation;
namespace asr = renderer;

const MString RenderGlobalsNode::nodeName("appleseedRenderGlobals");
const MTypeId RenderGlobalsNode::id(RenderGlobalsNodeTypeId);

MObject RenderGlobalsNode::m_pixelSamples;
MObject RenderGlobalsNode::m_passes;
MObject RenderGlobalsNode::m_tileSize;

MObject RenderGlobalsNode::m_lightingEngine;

MStringArray RenderGlobalsNode::m_diagnosticShaderKeys;
MObject RenderGlobalsNode::m_diagnosticShader;

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

MObject RenderGlobalsNode::m_imageFormat;

void* RenderGlobalsNode::creator()
{
    return new RenderGlobalsNode();
}

MStatus RenderGlobalsNode::initialize()
{
    MFnNumericAttribute numAttrFn;
    MFnMessageAttribute msgAttrFn;

    MStatus status;

    // Pixel Samples.
    m_pixelSamples = numAttrFn.create("samples", "samples", MFnNumericData::kInt, 16, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals samples attribute");

    numAttrFn.setMin(1);
    status = addAttribute(m_pixelSamples);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals samples attribute");

    // Render Passes.
    m_passes = numAttrFn.create("passes", "passes", MFnNumericData::kInt, 1, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals passes attribute");

    numAttrFn.setMin(1);
    status = addAttribute(m_passes);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals passes attribute");

    // Tile Size.
    m_tileSize = numAttrFn.create("tileSize", "tileSize", MFnNumericData::kInt, 64, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals tileSize attribute");

    numAttrFn.setMin(1);
    status = addAttribute(m_tileSize);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals tileSize attribute");

    // Lighting engine.
    MFnEnumAttribute enumAttrFn;
    m_lightingEngine = enumAttrFn.create("lightingEngine", "lightingEngine", 0, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals lighting engine attribute");

    enumAttrFn.addField("Path Tracing", 0);

    status = addAttribute(m_lightingEngine);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals lighting engine attribute");

    // Diagnostic shader override.
    m_diagnosticShader = enumAttrFn.create("diagnostics", "diagnostics", 0, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals diagnostic shader attribute");
    {
        short menuIndex = 0;
        enumAttrFn.addField("No Override", menuIndex++);
        m_diagnosticShaderKeys.append("no_override");

        asr::SurfaceShaderFactoryRegistrar factoryRegistrar;
        const asr::ISurfaceShaderFactory *factory = factoryRegistrar.lookup("diagnostic_surface_shader");
        assert(factory);
        asf::DictionaryArray metadata = factory->get_input_metadata();
        const asf::Dictionary& items = metadata[0].dictionary("items");

        asf::StringDictionary::const_iterator it(items.strings().begin());
        asf::StringDictionary::const_iterator e(items.strings().end());
        for(; it != e; ++it)
        {
            enumAttrFn.addField(it.key(), menuIndex++);
            m_diagnosticShaderKeys.append(MString(it.value()));
        }
    }

    status = addAttribute(m_diagnosticShader);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals diagnostic shader attribute");

    // Limit bounces.
    m_limitBounces = numAttrFn.create("limitBounces", "limitBounces", MFnNumericData::kBoolean, false, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals limitBounces attribute");

    status = addAttribute(m_limitBounces);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals limitBounces attribute");

    // Global Bounces.
    m_globalBounces = numAttrFn.create("bounces", "bounces", MFnNumericData::kInt, 8, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals bounces attribute");

    numAttrFn.setMin(0);
    status = addAttribute(m_globalBounces);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals bounces attribute");

    // Specular Bounces.
    m_specularBounces = numAttrFn.create("specularBounces", "specularBounces", MFnNumericData::kInt, 8, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals specular bounces attribute");

    numAttrFn.setMin(0);
    status = addAttribute(m_specularBounces);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals specular bounces attribute");

    // Glossy Bounces.
    m_glossyBounces = numAttrFn.create("glossyBounces", "glossyBounces", MFnNumericData::kInt, 8, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals glossy bounces attribute");

    numAttrFn.setMin(0);
    status = addAttribute(m_glossyBounces);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals glossy bounces attribute");

    // Diffuse Bounces.
    m_diffuseBounces = numAttrFn.create("diffuseBounces", "diffuseBounces", MFnNumericData::kInt, 8, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals diffuse bounces attribute");

    numAttrFn.setMin(0);
    status = addAttribute(m_diffuseBounces);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals diffuse bounces attribute");

    // Light Samples.
    m_lightSamples = numAttrFn.create("lightSamples", "lightSamples", MFnNumericData::kFloat, 1.0f, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals lightSamples attribute");
    numAttrFn.setMin(1.0f);
    status = addAttribute(m_lightSamples);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals lightSamples attribute");

    // Environment Samples.
    m_envSamples = numAttrFn.create("envSamples", "envSamples", MFnNumericData::kFloat, 1.0f, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals envSamples attribute");
    numAttrFn.setMin(1.0f);
    status = addAttribute(m_envSamples);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals envSamples attribute");

    // Caustics.
    m_caustics = numAttrFn.create("caustics", "caustics", MFnNumericData::kBoolean, false, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals caustics attribute");

    status = addAttribute(m_caustics);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals caustics attribute");

    // Max Ray Intensity.
    m_maxRayIntensity = numAttrFn.create("maxRayIntensity", "maxRayIntensity", MFnNumericData::kFloat, 0.0f, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals maxRayIntensity attribute");
    numAttrFn.setMin(0.0f);
    status = addAttribute(m_maxRayIntensity);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals maxRayIntensity attribute");

    // Background emits light.
    m_backgroundEmitsLight = numAttrFn.create("bgLight", "bgLight", MFnNumericData::kBoolean, true, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals bgLight attribute");

    status = addAttribute(m_backgroundEmitsLight);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals bgLight attribute");

    // Motion blur enable.
    m_motionBlur = numAttrFn.create("motionBlur", "motionBlur", MFnNumericData::kBoolean, false, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals motion blur attribute");

    status = addAttribute(m_motionBlur);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals motion blur attribute");

    m_mbCameraSamples = numAttrFn.create("mbCameraSamples", "mbCameraSamples", MFnNumericData::kInt, 2, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals mb camera samples attribute");

    numAttrFn.setMin(1);
    status = addAttribute(m_mbCameraSamples);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals mb camera samples attribute");

    m_mbTransformSamples = numAttrFn.create("mbTransformSamples", "mbTransformSamples", MFnNumericData::kInt, 2, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals mb transform samples attribute");

    numAttrFn.setMin(1);
    status = addAttribute(m_mbTransformSamples);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals mb transform samples attribute");

    m_mbDeformSamples = numAttrFn.create("mbDeformSamples", "mbDeformSamples", MFnNumericData::kInt, 2, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals mb deform samples attribute");

    numAttrFn.setMin(1);
    status = addAttribute(m_mbDeformSamples);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals mb deform samples attribute");

    // Shutter Open.
    m_shutterOpen = numAttrFn.create("shutterOpen", "shutterOpen", MFnNumericData::kFloat, -0.25f, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals shutterOpen attribute");
    status = addAttribute(m_shutterOpen);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals shutterOpen attribute");

    // Shutter Close.
    m_shutterClose = numAttrFn.create("shutterClose", "shutterClose", MFnNumericData::kFloat, 0.25f, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals shutterClose attribute");
    status = addAttribute(m_shutterClose);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals shutterClose attribute");

    // Rendering threads.
    m_renderingThreads = numAttrFn.create("threads", "threads", MFnNumericData::kInt, 0, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals threads attribute");

    status = addAttribute(m_renderingThreads);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals threads attribute");

    // Environment light connection.
    m_envLightNode = msgAttrFn.create("envLight", "env", &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals envLight attribute");

    status = addAttribute(m_envLightNode);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals envLight attribute");

    // Image Format
    m_imageFormat = numAttrFn.create("imageFormat", "imageFormat", MFnNumericData::kInt, 0, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals imageFormat attribute");

    status = addAttribute(m_imageFormat);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals imageFormat attribute");

    return status;
}

MStatus RenderGlobalsNode::compute(const MPlug& plug, MDataBlock& dataBlock)
{
    return MS::kSuccess;
}

void RenderGlobalsNode::applyGlobalsToProject(
    const MObject&                      globals,
    asr::Project&                       project)
{
    asr::ParamArray& finalParams = project.configurations().get_by_name("final")->get_parameters();
    asr::ParamArray& iprParams   = project.configurations().get_by_name("interactive")->get_parameters();

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

    int diagnostic;
    if (AttributeUtils::get(MPlug(globals, m_diagnosticShader), diagnostic))
    {
        if (diagnostic != 0)
        {
            finalParams.insert_path(
                "shading_engine.override_shading.mode",
                m_diagnosticShaderKeys[diagnostic]);
            iprParams.insert_path(
                "shading_engine.override_shading.mode",
                m_diagnosticShaderKeys[diagnostic]);
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
        {
            finalParams.insert_path("pt.max_bounces", bounces);
            iprParams.insert_path("pt.max_bounces", bounces);
        }

        bounces = -1;
        if (AttributeUtils::get(MPlug(globals, m_specularBounces), bounces))
        {
            finalParams.insert_path("pt.max_specular_bounces", bounces);
            iprParams.insert_path("pt.max_specular_bounces", bounces);
        }

        bounces = -1;
        if (AttributeUtils::get(MPlug(globals, m_glossyBounces), bounces))
        {
            finalParams.insert_path("pt.max_glossy_bounces", bounces);
            iprParams.insert_path("pt.max_glossy_bounces", bounces);
        }

        bounces = -1;
        if (AttributeUtils::get(MPlug(globals, m_diffuseBounces), bounces))
        {
            finalParams.insert_path("pt.max_diffuse_bounces", bounces);
            iprParams.insert_path("pt.max_diffuse_bounces", bounces);
        }
    }

    bool caustics;
    if (AttributeUtils::get(MPlug(globals, m_caustics), caustics))
    {
        finalParams.insert_path("pt.enable_caustics", caustics);
        iprParams.insert_path("pt.enable_caustics", caustics);
    }

    float maxRayIntensity;
    if (AttributeUtils::get(MPlug(globals, m_maxRayIntensity), maxRayIntensity))
    {
        if (maxRayIntensity == 0.0f)
        {
            finalParams.remove_path("pt.max_ray_intensity");
            iprParams.remove_path("pt.max_ray_intensity");
        }
        else
        {
            finalParams.insert_path("pt.max_ray_intensity", maxRayIntensity);
            iprParams.insert_path("pt.max_ray_intensity", maxRayIntensity);
        }
    }


    float lightSamples;
    if (AttributeUtils::get(MPlug(globals, m_lightSamples), lightSamples))
    {
        finalParams.insert_path("pt.dl_light_samples", lightSamples);
        iprParams.insert_path("pt.dl_light_samples", lightSamples);
    }

    float envSamples;
    if (AttributeUtils::get(MPlug(globals, m_envSamples), envSamples))
    {
        finalParams.insert_path("pt.ibl_env_samples", envSamples);
        iprParams.insert_path("pt.ibl_env_samples", envSamples);
    }

    int threads;
    if (AttributeUtils::get(MPlug(globals, m_renderingThreads), threads))
    {
        if (threads == 0)
        {
            finalParams.insert_path("rendering_threads", "auto");
            iprParams.insert_path("rendering_threads", "auto");
        }
        else
        {
            finalParams.insert_path("rendering_threads", threads);
            iprParams.insert_path("rendering_threads", threads);
        }
    }
}

void RenderGlobalsNode::collectMotionBlurTimes(
    const MObject&                      globals,
    AppleseedSession::MotionBlurTimes&  motionBlurTimes)
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

        motionBlurTimes.clear();
        motionBlurTimes.m_shutterOpenTime = now + shutterOpenTime;
        motionBlurTimes.m_shutterCloseTime = now + shutterCloseTime;

        int cameraSamples = 1;
        if (AttributeUtils::get(MPlug(globals, m_mbCameraSamples), cameraSamples))
        {
            motionBlurTimes.initializeFrameSet(
                cameraSamples,
                motionBlurTimes.m_shutterOpenTime,
                motionBlurTimes.m_shutterCloseTime,
                motionBlurTimes.m_cameraTimes);
        }

        int xformSamples = 1;
        if (AttributeUtils::get(MPlug(globals, m_mbTransformSamples), xformSamples))
        {
            motionBlurTimes.initializeFrameSet(
                xformSamples,
                motionBlurTimes.m_shutterOpenTime,
                motionBlurTimes.m_shutterCloseTime,
                motionBlurTimes.m_transformTimes);
        }

        int deformSamples = 1;
        if (AttributeUtils::get(MPlug(globals, m_mbDeformSamples), deformSamples))
        {
            if (!asf::is_pow2(deformSamples))
                deformSamples = asf::next_pow2(deformSamples);

            motionBlurTimes.initializeFrameSet(
                deformSamples,
                motionBlurTimes.m_shutterOpenTime,
                motionBlurTimes.m_shutterCloseTime,
                motionBlurTimes.m_deformTimes);
        }

        motionBlurTimes.mergeTimes();
    }
    else
    {
        motionBlurTimes.initializeToCurrentFrame();
    }
}
