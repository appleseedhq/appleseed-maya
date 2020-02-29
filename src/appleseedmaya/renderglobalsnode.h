
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

#ifndef APPLESEED_MAYA_RENDERGLOBALSNODE_H
#define APPLESEED_MAYA_RENDERGLOBALSNODE_H

// appleseed-maya headers.
#include "appleseedmaya/appleseedsession.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/aov.h"
#include "renderer/api/log.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MObject.h>
#include <maya/MPxNode.h>
#include <maya/MStringArray.h>
#include <maya/MTypeId.h>
#include "appleseedmaya/_endmayaheaders.h"

// Forward declarations.
namespace renderer { class Project; }

class RenderGlobalsNode
  : public MPxNode
{
  public:
    static const MString nodeName;
    static const MTypeId id;

    static void* creator();
    static MStatus initialize();

    MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;

    static void applyGlobalsToProject(
        const MObject&                              globals,
        AppleseedSession::SessionMode               sessionMode,
        renderer::Project&                          project,
        renderer::AOVContainer&                     aovs);

    static void applyPostProcessStagesToFrame(
        const MObject&                              globals,
        renderer::Project&                          project);

    static void collectMotionBlurSampleTimes(
        const MObject&                              globals,
        AppleseedSession::MotionBlurSampleTimes&    motionBlurSampleTimes);

    static foundation::LogMessage::Category logLevel(const MObject& globals);
    static MString logFilename(const MObject& globals);

  private:
    static MObject      m_passes;

    // Adaptive tile sampler.
    static MObject      m_adaptiveSampling;
    static MObject      m_minPixelSamples;
    static MObject      m_maxPixelSamples;
    static MObject      m_batchSampleSize;
    static MObject      m_sampleNoiseThreshold;

    static MObject      m_tileSize;

    // Pixel filter.
    static MObject      m_pixelFilter;
    static MStringArray m_pixelFilterKeys;
    static MObject      m_pixelFilterSize;

    static MObject      m_sceneScale;

    // Lighting.
    static MObject      m_lightingEngine;
    static MObject      m_lightSamplingAlgorithm;
    static MObject      m_enableLightImportanceSampling;

    // Shading overrides.
    static MObject      m_diagnosticShader;
    static MStringArray m_diagnosticShaderKeys;

    // Noise seed.
    static MObject      m_lockSamplingPattern;
    static MObject      m_noiseSeed;

    // Path tracer.
    static MObject      m_enableDirectLighting;
    static MObject      m_enableIBL;
    static MObject      m_limitBounces;
    static MObject      m_globalBounces;
    static MObject      m_specularBounces;
    static MObject      m_glossyBounces;
    static MObject      m_diffuseBounces;

    static MObject      m_lightSamples;
    static MObject      m_envSamples;
    static MObject      m_caustics;
    static MObject      m_enableMaxRayIntensity;
    static MObject      m_maxRayIntensity;
    static MObject      m_clampRoughness;
    static MObject      m_lowLightThreshold;

    // Environment and image-based lighting.
    static MObject      m_envLightNode;
    static MObject      m_backgroundEmitsLight;

    // Stochastic progressive photon mapping.
    static MObject      m_sppm_photon_type;
    static MObject      m_sppm_direct_lighting_mode;
    static MObject      m_sppm_enable_caustics;
    static MObject      m_sppm_enable_ibl;
    static MObject      m_sppm_photon_tracing_enable_bounce_limit;
    static MObject      m_sppm_photon_tracing_max_bounces;
    static MObject      m_sppm_photon_tracing_rr_min_path_length;
    static MObject      m_sppm_photon_tracing_light_photons;
    static MObject      m_sppm_photon_tracing_environment_photons;
    static MObject      m_sppm_radiance_estimation_enable_bounce_limit;
    static MObject      m_sppm_radiance_estimation_max_bounces;
    static MObject      m_sppm_radiance_estimation_rr_min_path_length;
    static MObject      m_sppm_radiance_estimation_initial_radius;
    static MObject      m_sppm_radiance_estimation_max_photons;
    static MObject      m_sppm_radiance_estimation_alpha;
    static MObject      m_sppm_max_ray_intensity_set;
    static MObject      m_sppm_max_ray_intensity;
    static MObject      m_sppm_enable_importons;

    // Motion blur.
    static MObject      m_motionBlur;
    static MObject      m_mbCameraSamples;
    static MObject      m_mbTransformSamples;
    static MObject      m_mbDeformSamples;
    static MObject      m_shutterOpen;
    static MObject      m_shutterClose;

    // System settings.
    static MObject      m_renderingThreads;
    static MObject      m_maxTextureCacheSize;

    // Experimental.
    static MObject      m_useEmbree;

    // Denoiser.
    static MObject      m_denoiserMode;
    static MStringArray m_denoiserModeKeys;

    static MObject      m_skipDenoisedPixels;
    static MObject      m_randomPixelOrder;
    static MObject      m_prefilterSpikes;
    static MObject      m_spikeThreshold;
    static MObject      m_patchDistanceThreshold;
    static MObject      m_denoiseScales;

    static MObject      m_imageFormat;

    // AOVs.
    static MObject      m_albedoAOV;
    static MObject      m_cryptomatteMaterialAOV;
    static MObject      m_cryptomatteObjectAOV;
    static MObject      m_depthAOV;
    static MObject      m_diffuseAOV;
    static MObject      m_directDiffuseAOV;
    static MObject      m_directGlossyAOV;
    static MObject      m_emissionAOV;
    static MObject      m_glossyAOV;
    static MObject      m_indirectDiffuseAOV;
    static MObject      m_indirectGlossyAOV;
    static MObject      m_invalidSamplesAOV;
    static MObject      m_normalAOV;
    static MObject      m_nprContourAOV;
    static MObject      m_nprShadingAOV;
    static MObject      m_pixelErrorAOV;
    static MObject      m_pixelSampleCountAOV;
    static MObject      m_pixelTimeAOV;
    static MObject      m_pixelVariationAOV;
    static MObject      m_positionAOV;
    static MObject      m_uvAOV;
    static MObject      m_velocityAOV;

    // Render stamp.
    static MObject      m_renderStamp;
    static MObject      m_renderStampString;

    // Logging.
    static MObject      m_logLevel;
    static MObject      m_logFilename;
};

#endif  // !APPLESEED_MAYA_RENDERGLOBALSNODE_H
