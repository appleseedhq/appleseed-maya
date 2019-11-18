
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
#include "renderglobalsnode.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/config.h"
#include "appleseedmaya/typeids.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/frame.h"
#include "renderer/api/postprocessing.h"
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
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnStringData.h>
#include "appleseedmaya/_endmayaheaders.h"

namespace asf = foundation;
namespace asr = renderer;

const MString RenderGlobalsNode::nodeName("appleseedRenderGlobals");
const MTypeId RenderGlobalsNode::id(RenderGlobalsNodeTypeId);

MObject RenderGlobalsNode::m_passes;

MObject RenderGlobalsNode::m_adaptiveSampling;
MObject RenderGlobalsNode::m_minPixelSamples;
MObject RenderGlobalsNode::m_maxPixelSamples;
MObject RenderGlobalsNode::m_batchSampleSize;
MObject RenderGlobalsNode::m_sampleNoiseThreshold;

MObject RenderGlobalsNode::m_tileSize;
MObject RenderGlobalsNode::m_pixelFilter;
MStringArray RenderGlobalsNode::m_pixelFilterKeys;
MObject RenderGlobalsNode::m_pixelFilterSize;

MObject RenderGlobalsNode::m_lockSamplingPattern;
MObject RenderGlobalsNode::m_noiseSeed;

MObject RenderGlobalsNode::m_sceneScale;

MObject RenderGlobalsNode::m_lightingEngine;
MObject RenderGlobalsNode::m_lightSamplingAlgorithm;
MObject RenderGlobalsNode::m_enableLightImportanceSampling;

MObject RenderGlobalsNode::m_diagnosticShader;
MStringArray RenderGlobalsNode::m_diagnosticShaderKeys;

MObject RenderGlobalsNode::m_enableDirectLighting;
MObject RenderGlobalsNode::m_enableIBL;
MObject RenderGlobalsNode::m_limitBounces;
MObject RenderGlobalsNode::m_globalBounces;
MObject RenderGlobalsNode::m_specularBounces;
MObject RenderGlobalsNode::m_glossyBounces;
MObject RenderGlobalsNode::m_diffuseBounces;
MObject RenderGlobalsNode::m_lightSamples;
MObject RenderGlobalsNode::m_envSamples;
MObject RenderGlobalsNode::m_caustics;
MObject RenderGlobalsNode::m_enableMaxRayIntensity;
MObject RenderGlobalsNode::m_maxRayIntensity;
MObject RenderGlobalsNode::m_clampRoughness;
MObject RenderGlobalsNode::m_lowLightThreshold;

MObject RenderGlobalsNode::m_sppm_photon_type;
MObject RenderGlobalsNode::m_sppm_direct_lighting_mode;
MObject RenderGlobalsNode::m_sppm_enable_caustics;
MObject RenderGlobalsNode::m_sppm_enable_ibl;
MObject RenderGlobalsNode::m_sppm_photon_tracing_enable_bounce_limit;
MObject RenderGlobalsNode::m_sppm_photon_tracing_max_bounces;
MObject RenderGlobalsNode::m_sppm_photon_tracing_rr_min_path_length;
MObject RenderGlobalsNode::m_sppm_photon_tracing_light_photons;
MObject RenderGlobalsNode::m_sppm_photon_tracing_environment_photons;
MObject RenderGlobalsNode::m_sppm_radiance_estimation_enable_bounce_limit;
MObject RenderGlobalsNode::m_sppm_radiance_estimation_max_bounces;
MObject RenderGlobalsNode::m_sppm_radiance_estimation_rr_min_path_length;
MObject RenderGlobalsNode::m_sppm_radiance_estimation_initial_radius;
MObject RenderGlobalsNode::m_sppm_radiance_estimation_max_photons;
MObject RenderGlobalsNode::m_sppm_radiance_estimation_alpha;
MObject RenderGlobalsNode::m_sppm_max_ray_intensity_set;
MObject RenderGlobalsNode::m_sppm_max_ray_intensity;

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

MObject RenderGlobalsNode::m_useEmbree;

MObject RenderGlobalsNode::m_denoiserMode;
MStringArray RenderGlobalsNode::m_denoiserModeKeys;

MObject RenderGlobalsNode::m_skipDenoisedPixels;
MObject RenderGlobalsNode::m_randomPixelOrder;
MObject RenderGlobalsNode::m_prefilterSpikes;
MObject RenderGlobalsNode::m_spikeThreshold;
MObject RenderGlobalsNode::m_patchDistanceThreshold;
MObject RenderGlobalsNode::m_denoiseScales;

MObject RenderGlobalsNode::m_imageFormat;

MObject RenderGlobalsNode::m_albedoAOV;
MObject RenderGlobalsNode::m_cryptomatteMaterialAOV;
MObject RenderGlobalsNode::m_cryptomatteObjectAOV;
MObject RenderGlobalsNode::m_depthAOV;
MObject RenderGlobalsNode::m_diffuseAOV;
MObject RenderGlobalsNode::m_directDiffuseAOV;
MObject RenderGlobalsNode::m_directGlossyAOV;
MObject RenderGlobalsNode::m_emissionAOV;
MObject RenderGlobalsNode::m_glossyAOV;
MObject RenderGlobalsNode::m_indirectDiffuseAOV;
MObject RenderGlobalsNode::m_indirectGlossyAOV;
MObject RenderGlobalsNode::m_invalidSamplesAOV;
MObject RenderGlobalsNode::m_normalAOV;
MObject RenderGlobalsNode::m_nprContourAOV;
MObject RenderGlobalsNode::m_nprShadingAOV;
MObject RenderGlobalsNode::m_pixelErrorAOV;
MObject RenderGlobalsNode::m_pixelSampleCountAOV;
MObject RenderGlobalsNode::m_pixelTimeAOV;
MObject RenderGlobalsNode::m_pixelVariationAOV;
MObject RenderGlobalsNode::m_positionAOV;
MObject RenderGlobalsNode::m_uvAOV;
MObject RenderGlobalsNode::m_velocityAOV;

MObject RenderGlobalsNode::m_renderStamp;
MObject RenderGlobalsNode::m_renderStampString;

MObject RenderGlobalsNode::m_logLevel;
MObject RenderGlobalsNode::m_logFilename;

namespace
{

const asf::Dictionary* findDictionary(const asf::DictionaryArray& dictionaries, const char* name)
{
    for (size_t i = 0, e = dictionaries.size(); i < e; ++i)
    {
        const asf::Dictionary& dict = dictionaries[i];

        if (strcmp(dict.get("name"), name) == 0)
            return &dict;
    }

    return nullptr;
}

}

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
    MFnTypedAttribute typedAttrFn;
    MFnStringData stringDataFn;

    MStatus status;

    // Render passes.
    m_passes = numAttrFn.create("passes", "passes", MFnNumericData::kInt, 1, &status);
    numAttrFn.setMin(1);
    numAttrFn.setSoftMax(64);
    CHECKED_ADD_ATTRIBUTE(m_passes, "passes")

    // Adaptive sampling.
    m_adaptiveSampling = numAttrFn.create("adaptiveSampling", "adaptiveSampling", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_adaptiveSampling, "adaptiveSampling")


    // Min pixel samples.
    m_minPixelSamples = numAttrFn.create("minPixelSamples", "minPixelSamples", MFnNumericData::kInt, 16, &status);
    numAttrFn.setMin(0);
    numAttrFn.setSoftMax(1024);
    CHECKED_ADD_ATTRIBUTE(m_minPixelSamples, "minPixelSamples")

    // Max pixel samples.
    m_maxPixelSamples = numAttrFn.create("samples", "samples", MFnNumericData::kInt, 32, &status);
    numAttrFn.setMin(0);
    numAttrFn.setSoftMax(1024);
    CHECKED_ADD_ATTRIBUTE(m_maxPixelSamples, "samples")

    // Batch sample size.
    m_batchSampleSize = numAttrFn.create("batchSampleSize", "batchSampleSize", MFnNumericData::kInt, 16, &status);
    numAttrFn.setMin(1);
    numAttrFn.setSoftMax(256);
    CHECKED_ADD_ATTRIBUTE(m_batchSampleSize, "batchSampleSize")

    // Sample noise threshold.
    m_sampleNoiseThreshold = numAttrFn.create("sampleNoiseThreshold", "sampleNoiseThreshold", MFnNumericData::kFloat, 0.1, &status);
    numAttrFn.setMin(0.0);
    numAttrFn.setMax(10000.0);
    CHECKED_ADD_ATTRIBUTE(m_sampleNoiseThreshold, "sampleNoiseThreshold")

    // Tile size.
    m_tileSize = numAttrFn.create("tileSize", "tileSize", MFnNumericData::kInt, 64, &status);
    numAttrFn.setMin(1);
    numAttrFn.setMax(512);
    numAttrFn.setKeyable(false);
    numAttrFn.setConnectable(false);

    CHECKED_ADD_ATTRIBUTE(m_tileSize, "tileSize")

    // Pixel filter.
    m_pixelFilter = enumAttrFn.create("pixelFilter", "pixelFilter", 0, &status);
    {
        asf::DictionaryArray metadata = asr::FrameFactory().get_input_metadata();

        const asf::Dictionary* filter_metadata = findDictionary(metadata, "filter");
        const asf::Dictionary& items = filter_metadata->dictionary("items");

        short menuIndex = 0;

        for (auto it(items.strings().begin()), e(items.strings().end()); it != e; ++it)
        {
            enumAttrFn.addField(it.key(), menuIndex++);
            m_pixelFilterKeys.append(MString(it.value()));
        }
    }
    CHECKED_ADD_ATTRIBUTE(m_pixelFilter, "pixelFilter")

    // Pixel filter size.
    m_pixelFilterSize = numAttrFn.create("pixelFilterSize", "pixelFilterSize", MFnNumericData::kFloat, 1.5, &status);
    numAttrFn.setMin(0.5);
    numAttrFn.setSoftMax(4.0);
    CHECKED_ADD_ATTRIBUTE(m_pixelFilterSize, "pixelFilterSize")

    // Scene scale.
    m_sceneScale = numAttrFn.create("sceneScale", "sceneScale", MFnNumericData::kFloat, 1.0, &status);
    numAttrFn.setMin(0.0000001);
    numAttrFn.setSoftMax(1000.0);
    CHECKED_ADD_ATTRIBUTE(m_sceneScale, "sceneScale")

    // Lighting engine.
    m_lightingEngine = enumAttrFn.create("lightingEngine", "lightingEngine", 0, &status);
    enumAttrFn.addField("Path Tracing", 0);
    enumAttrFn.addField("Stochastic Progressive Photon Mapping", 1);
    CHECKED_ADD_ATTRIBUTE(m_lightingEngine, "lightingEngine")

    // Light sampling algorithm.
    m_lightSamplingAlgorithm = enumAttrFn.create("lightSamplingAlgorithm", "lightSamplingAlgorithm", 0, &status);
    enumAttrFn.addField("CDF", 0);
    enumAttrFn.addField("Light Tree", 1);
    CHECKED_ADD_ATTRIBUTE(m_lightSamplingAlgorithm, "lightSamplingAlgorithm")

    // Light importance sampling.
    m_enableLightImportanceSampling = numAttrFn.create("lightImportanceSampling", "lightImportanceSampling", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_enableLightImportanceSampling, "lightImportanceSampling")

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

    // Disables noise seed variation per frame.
    m_lockSamplingPattern = numAttrFn.create("lockSamplingPattern", "lockSamplingPattern", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_lockSamplingPattern, "lockSamplingPattern")

    // Noise seed value.
    m_noiseSeed = numAttrFn.create("noiseSeed", "noiseSeed", MFnNumericData::kInt, 0, &status);
    numAttrFn.setMin(std::numeric_limits<int>::min());
    numAttrFn.setMax(std::numeric_limits<int>::max());
    CHECKED_ADD_ATTRIBUTE(m_noiseSeed, "noiseSeed")

    //
    // Path Tracing
    //

    // Direct lighting.
    m_enableDirectLighting = numAttrFn.create("enableDirectLighting", "enableDirectLighting", MFnNumericData::kBoolean, true, &status);
    CHECKED_ADD_ATTRIBUTE(m_enableDirectLighting, "enableDirectLighting")

    // Image based lighting.
    m_enableIBL = numAttrFn.create("enableIBL", "enableIBL", MFnNumericData::kBoolean, true, &status);
    CHECKED_ADD_ATTRIBUTE(m_enableIBL, "enableIBL")

    // Limit bounces.
    m_limitBounces = numAttrFn.create("limitBounces", "limitBounces", MFnNumericData::kBoolean, true, &status);
    CHECKED_ADD_ATTRIBUTE(m_limitBounces, "limitBounces")

    // Global bounces.
    m_globalBounces = numAttrFn.create("bounces", "bounces", MFnNumericData::kInt, 8, &status);
    numAttrFn.setMin(0);
    numAttrFn.setSoftMax(32);
    CHECKED_ADD_ATTRIBUTE(m_globalBounces, "bounces")

    // Specular bounces.
    m_specularBounces = numAttrFn.create("specularBounces", "specularBounces", MFnNumericData::kInt, 8, &status);
    numAttrFn.setMin(0);
    numAttrFn.setSoftMax(32);
    CHECKED_ADD_ATTRIBUTE(m_specularBounces, "specularBounces")

    // Glossy bounces.
    m_glossyBounces = numAttrFn.create("glossyBounces", "glossyBounces", MFnNumericData::kInt, 8, &status);
    numAttrFn.setMin(0);
    numAttrFn.setSoftMax(32);
    CHECKED_ADD_ATTRIBUTE(m_glossyBounces, "glossyBounces")

    // Diffuse bounces.
    m_diffuseBounces = numAttrFn.create("diffuseBounces", "diffuseBounces", MFnNumericData::kInt, 3, &status);
    numAttrFn.setMin(0);
    numAttrFn.setSoftMax(32);
    CHECKED_ADD_ATTRIBUTE(m_diffuseBounces, "diffuseBounces")

    // Light samples.
    m_lightSamples = numAttrFn.create("lightSamples", "lightSamples", MFnNumericData::kFloat, 1.0, &status);
    numAttrFn.setMin(1.0);
    numAttrFn.setSoftMax(16.0);
    CHECKED_ADD_ATTRIBUTE(m_lightSamples, "lightSamples")

    // Environment samples.
    m_envSamples = numAttrFn.create("envSamples", "envSamples", MFnNumericData::kFloat, 1.0, &status);
    numAttrFn.setMin(1.0);
    numAttrFn.setSoftMax(16.0);
    CHECKED_ADD_ATTRIBUTE(m_envSamples, "envSamples")

    // Caustics.
    m_caustics = numAttrFn.create("caustics", "caustics", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_caustics, "caustics")

    // Enable max ray intensity.
    m_enableMaxRayIntensity = numAttrFn.create("enableMaxRayIntensity", "enableMaxRayIntensity", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_enableMaxRayIntensity, "enableMaxRayIntensity")

    // Max ray intensity.
    m_maxRayIntensity = numAttrFn.create("maxRayIntensity", "maxRayIntensity", MFnNumericData::kFloat, 1.0, &status);
    numAttrFn.setMin(0.0);
    CHECKED_ADD_ATTRIBUTE(m_maxRayIntensity, "maxRayIntensity")

    // Roughness clamping. 
    m_clampRoughness = numAttrFn.create("clampRoughness", "clampRoughness", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_clampRoughness, "clampRoughness")

    // Background visibility.
    m_backgroundEmitsLight = numAttrFn.create("bgLight", "bgLight", MFnNumericData::kBoolean, true, &status);
    CHECKED_ADD_ATTRIBUTE(m_backgroundEmitsLight, "bgLight")

    // Low light threshold.
    m_lowLightThreshold = numAttrFn.create("lowLightThreshold", "lowLightThreshold", MFnNumericData::kFloat, 0.0, &status);
    numAttrFn.setMin(0.0);
    numAttrFn.setMax(1000.0);
    CHECKED_ADD_ATTRIBUTE(m_lowLightThreshold, "lowLightThreshold")

    //
    // Stochastic Progressive Photon Mapping
    //

    // Photon type.
    m_sppm_photon_type = enumAttrFn.create("photonType", "photonType", 0, &status);
    enumAttrFn.addField("Polychromatic", 0);
    enumAttrFn.addField("Monochromatic", 1);
    CHECKED_ADD_ATTRIBUTE(m_sppm_photon_type, "photonType")

    // Direct lighting mode.
    m_sppm_direct_lighting_mode = enumAttrFn.create("SPPMLightingMode", "SPPMLightingMode", 0, &status);
    enumAttrFn.addField("Ray Traced", 0);
    enumAttrFn.addField("SPPM", 1);
    enumAttrFn.addField("No Direct Lighting", 2);
    CHECKED_ADD_ATTRIBUTE(m_sppm_direct_lighting_mode, "SPPMLightingMode")

    // SPPM Caustics.
    m_sppm_enable_caustics = numAttrFn.create("SPPMCaustics", "SPPMCaustics", MFnNumericData::kBoolean, true, &status);
    CHECKED_ADD_ATTRIBUTE(m_sppm_enable_caustics, "SPPMCaustics")

    // SPPM IBL.
    m_sppm_enable_ibl = numAttrFn.create("SPPMEnableIBL", "SPPMEnableIBL", MFnNumericData::kBoolean, true, &status);
    CHECKED_ADD_ATTRIBUTE(m_sppm_enable_ibl, "SPPMEnableIBL")

    // Limit photon tracing bounces.
    m_sppm_photon_tracing_enable_bounce_limit = numAttrFn.create("limitPhotonTracingBounces", "limitPhotonTracingBounces", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_sppm_photon_tracing_enable_bounce_limit, "limitPhotonTracingBounces")

    // Photon tracing bounces.
    m_sppm_photon_tracing_max_bounces = numAttrFn.create("photonTracingBounces", "photonTracingBounces", MFnNumericData::kInt, 8, &status);
    numAttrFn.setMin(0);
    numAttrFn.setSoftMax(32);
    CHECKED_ADD_ATTRIBUTE(m_sppm_photon_tracing_max_bounces, "photonTracingBounces")

    // Photon tracing russian roulette start bounce.
    m_sppm_photon_tracing_rr_min_path_length = numAttrFn.create("photonTracingRRMinPathLength", "photonTracingRRMinPathLength", MFnNumericData::kInt, 6, &status);
    numAttrFn.setMin(1);
    numAttrFn.setSoftMax(32);
    CHECKED_ADD_ATTRIBUTE(m_sppm_photon_tracing_rr_min_path_length, "photonTracingRRMinPathLength")

    // Photon tracing light photons.
    m_sppm_photon_tracing_light_photons = numAttrFn.create("photonTracingLightPhotons", "photonTracingLightPhotons", MFnNumericData::kInt, 1000000, &status);
    numAttrFn.setMin(0);
    numAttrFn.setSoftMax(10000000);
    CHECKED_ADD_ATTRIBUTE(m_sppm_photon_tracing_light_photons, "photonTracingLightPhotons")

    // Photon tracing environment photons.
    m_sppm_photon_tracing_environment_photons = numAttrFn.create("photonTracingEnvPhotons", "photonTracingEnvPhotons", MFnNumericData::kInt, 1000000, &status);
    numAttrFn.setMin(0);
    numAttrFn.setSoftMax(10000000);
    CHECKED_ADD_ATTRIBUTE(m_sppm_photon_tracing_environment_photons, "photonTracingEnvPhotons")

    // Limit radiance estimation bounces.
    m_sppm_radiance_estimation_enable_bounce_limit = numAttrFn.create("limitRadianceEstimationBounces", "limitRadianceEstimationBounces", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_sppm_radiance_estimation_enable_bounce_limit, "limitRadianceEstimationBounces")

    // Radiance estimation bounces.
    m_sppm_radiance_estimation_max_bounces = numAttrFn.create("radianceEstimationBounces", "radianceEstimationBounces", MFnNumericData::kInt, 8, &status);
    numAttrFn.setMin(0);
    numAttrFn.setSoftMax(32);
    CHECKED_ADD_ATTRIBUTE(m_sppm_radiance_estimation_max_bounces, "radianceEstimationBounces")

    // Radiance estimation russian roulette start bounce.
    m_sppm_radiance_estimation_rr_min_path_length = numAttrFn.create("radianceEstimationRRMinPathLength", "radianceEstimationRRMinPathLength", MFnNumericData::kInt, 6, &status);
    numAttrFn.setMin(1);
    CHECKED_ADD_ATTRIBUTE(m_sppm_radiance_estimation_rr_min_path_length, "radianceEstimationRRMinPathLength")

    // Radiance estimation initial search radius.
    m_sppm_radiance_estimation_initial_radius = numAttrFn.create("radianceEstimationInitialRadius", "radianceEstimationInitialRadius", MFnNumericData::kFloat, 0.1, &status);
    numAttrFn.setMin(0.001);
    numAttrFn.setMax(100.0);
    CHECKED_ADD_ATTRIBUTE(m_sppm_radiance_estimation_initial_radius, "radianceEstimationInitialRadius")

    // Radiance estimation max photons.
    m_sppm_radiance_estimation_max_photons = numAttrFn.create("radianceEstimationMaxPhotons", "radianceEstimationMaxPhotons", MFnNumericData::kInt, 100, &status);
    numAttrFn.setMin(8);
    numAttrFn.setSoftMax(10000);
    CHECKED_ADD_ATTRIBUTE(m_sppm_radiance_estimation_max_photons, "radianceEstimationMaxPhotons")

    // Radiance estimation search radius reduction factor.
    m_sppm_radiance_estimation_alpha = numAttrFn.create("radianceEstimationAlpha", "radianceEstimationAlpha", MFnNumericData::kFloat, 0.7, &status);
    numAttrFn.setMin(0.0);
    numAttrFn.setMax(1.0);
    CHECKED_ADD_ATTRIBUTE(m_sppm_radiance_estimation_alpha, "radianceEstimationAlpha")

    // Enable ray intensity clamping SPPM.
    m_sppm_max_ray_intensity_set = numAttrFn.create("enableMaxRayIntensitySPPM", "enableMaxRayIntensitySPPM", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_sppm_max_ray_intensity_set, "enableMaxRayIntensitySPPM")

    // Max ray intensity SPPM.
    m_sppm_max_ray_intensity = numAttrFn.create("maxRayIntensitySPPM", "maxRayIntensitySPPM", MFnNumericData::kFloat, 1.0, &status);
    numAttrFn.setMin(0.0);
    numAttrFn.setMax(1000.0);
    CHECKED_ADD_ATTRIBUTE(m_sppm_max_ray_intensity, "maxRayIntensitySPPM")

    // Motion blur enable.
    m_motionBlur = numAttrFn.create("motionBlur", "motionBlur", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_motionBlur, "motionBlur")

    m_mbCameraSamples = numAttrFn.create("mbCameraSamples", "mbCameraSamples", MFnNumericData::kInt, 2, &status);
    numAttrFn.setMin(1);
    numAttrFn.setSoftMax(32);
    CHECKED_ADD_ATTRIBUTE(m_mbCameraSamples, "cameraSamples")

    m_mbTransformSamples = numAttrFn.create("mbTransformSamples", "mbTransformSamples", MFnNumericData::kInt, 2, &status);
    numAttrFn.setMin(1);
    numAttrFn.setSoftMax(32);
    CHECKED_ADD_ATTRIBUTE(m_mbTransformSamples, "xformSamples")

    m_mbDeformSamples = numAttrFn.create("mbDeformSamples", "mbDeformSamples", MFnNumericData::kInt, 2, &status);
    numAttrFn.setMin(1);
    numAttrFn.setSoftMax(32);
    CHECKED_ADD_ATTRIBUTE(m_mbDeformSamples, "deformSamples")

    // Shutter open.
    m_shutterOpen = numAttrFn.create("shutterOpen", "shutterOpen", MFnNumericData::kFloat, -0.25, &status);
    CHECKED_ADD_ATTRIBUTE(m_shutterOpen, "shutterOpen")

    // Shutter close.
    m_shutterClose = numAttrFn.create("shutterClose", "shutterClose", MFnNumericData::kFloat, 0.25, &status);
    CHECKED_ADD_ATTRIBUTE(m_shutterClose, "shutterClose")

    // Rendering threads.
    m_renderingThreads = numAttrFn.create("threads", "threads", MFnNumericData::kInt, -1, &status);
    CHECKED_ADD_ATTRIBUTE(m_renderingThreads, "threads")

    // Texture cache size.
    m_maxTextureCacheSize = numAttrFn.create("maxTexCacheSize", "maxTexCacheSize", MFnNumericData::kInt, 1024, &status);
    numAttrFn.setMin(16);
    CHECKED_ADD_ATTRIBUTE(m_maxTextureCacheSize, "maxTexCacheSize")

    // Embree.
    m_useEmbree = numAttrFn.create("useEmbree", "useEmbree", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_useEmbree, "useEmbree")

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

    // Skip already denoised pixels.
    m_skipDenoisedPixels = numAttrFn.create("skipDenoised", "skipDenoised", MFnNumericData::kBoolean, true, &status);
    CHECKED_ADD_ATTRIBUTE(m_skipDenoisedPixels, "skipDenoised")

    // Random pixel order.
    m_randomPixelOrder = numAttrFn.create("randomPixelOrder", "randomPixelOrder", MFnNumericData::kBoolean, true, &status);
    CHECKED_ADD_ATTRIBUTE(m_randomPixelOrder, "randomPixelOrder")

    // Prefilter spikes.
    m_prefilterSpikes = numAttrFn.create("prefilterSpikes", "prefilterSpikes", MFnNumericData::kBoolean, true, &status);
    CHECKED_ADD_ATTRIBUTE(m_prefilterSpikes, "prefilterSpikes")

    // Spike thereshold.
    m_spikeThreshold = numAttrFn.create("spikeThreshold", "spikeThreshold", MFnNumericData::kFloat, 2.0, &status);
    numAttrFn.setMin(0.1);
    numAttrFn.setMax(4.0);
    CHECKED_ADD_ATTRIBUTE(m_spikeThreshold, "spikeThreshold")

    // Patch distance.
    m_patchDistanceThreshold = numAttrFn.create("patchDistance", "patchDistance", MFnNumericData::kFloat, 1.0, &status);
    numAttrFn.setMin(0.5);
    numAttrFn.setMax(3.0);
    CHECKED_ADD_ATTRIBUTE(m_patchDistanceThreshold, "patchDistance")

    // Denoise scales.
    m_denoiseScales = numAttrFn.create("denoiseScales", "denoiseScales", MFnNumericData::kInt, 3, &status);
    numAttrFn.setMin(1);
    CHECKED_ADD_ATTRIBUTE(m_denoiseScales, "denoiseScales")

    // Image format
    m_imageFormat = numAttrFn.create("imageFormat", "imageFormat", MFnNumericData::kInt, 0, &status);
    CHECKED_ADD_ATTRIBUTE(m_imageFormat, "imageFormat")

    // AOVs.
    m_albedoAOV = numAttrFn.create("albedoAOV", "albedoAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_albedoAOV, "albedoAOV")

    m_cryptomatteMaterialAOV = numAttrFn.create("cryptomatteMaterialAOV", "cryptomatteMaterialAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_cryptomatteMaterialAOV, "cryptomatteMaterialAOV")

    m_cryptomatteObjectAOV = numAttrFn.create("cryptomatteObjectAOV", "cryptomatteObjectAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_cryptomatteObjectAOV, "cryptomatteObjectAOV")

    m_depthAOV = numAttrFn.create("depthAOV", "depthAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_depthAOV, "depthAOV")

    m_diffuseAOV = numAttrFn.create("diffuseAOV", "diffuseAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_diffuseAOV, "diffuseAOV")

    m_directDiffuseAOV = numAttrFn.create("directDiffuseAOV", "directDiffuseAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_directDiffuseAOV, "directDiffuseAOV")

    m_directGlossyAOV = numAttrFn.create("directGlossyAOV", "directGlossyAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_directGlossyAOV, "directGlossyAOV")

    m_emissionAOV = numAttrFn.create("emissionAOV", "emissionAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_emissionAOV, "emissionAOV")

    m_glossyAOV = numAttrFn.create("glossyAOV", "glossyAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_glossyAOV, "glossyAOV")

    m_indirectDiffuseAOV = numAttrFn.create("indirectDiffuseAOV", "indirectDiffuseAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_indirectDiffuseAOV, "indirectDiffuseAOV")

    m_indirectGlossyAOV = numAttrFn.create("indirectGlossyAOV", "indirectGlossyAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_indirectGlossyAOV, "indirectGlossyAOV")

    m_invalidSamplesAOV = numAttrFn.create("invalidSamplesAOV", "invalidSamplesAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_invalidSamplesAOV, "invalidSamplesAOV")

    m_normalAOV = numAttrFn.create("normalAOV", "normalAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_normalAOV, "normalAOV")

    m_nprContourAOV = numAttrFn.create("nprContourAOV", "nprContourAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_nprContourAOV, "nprContourAOV")

    m_nprShadingAOV = numAttrFn.create("nprShadingAOV", "nprShadingAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_nprShadingAOV, "nprShadingAOV")

    m_pixelErrorAOV = numAttrFn.create("pixelErrorAOV", "pixelErrorAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_pixelErrorAOV, "pixelErrorAOV")

    m_pixelSampleCountAOV = numAttrFn.create("pixelSampleCountAOV", "pixelSampleCountAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_pixelSampleCountAOV, "pixelSampleCountAOV")

    m_pixelTimeAOV = numAttrFn.create("pixelTimeAOV", "pixelTimeAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_pixelTimeAOV, "pixelTimeAOV")

    m_pixelVariationAOV = numAttrFn.create("pixelVariationAOV", "pixelVariationAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_pixelVariationAOV, "pixelVariationAOV")

    m_positionAOV = numAttrFn.create("positionAOV", "positionAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_positionAOV, "positionAOV")

    m_uvAOV = numAttrFn.create("uvAOV", "uvAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_uvAOV, "uvAOV")

    m_velocityAOV = numAttrFn.create("velocityAOV", "velocityAOV", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_velocityAOV, "velocityAOV")

    // Render stamp enable.
    m_renderStamp = numAttrFn.create("renderStamp", "renderStamp", MFnNumericData::kBoolean, false, &status);
    CHECKED_ADD_ATTRIBUTE(m_renderStamp, "renderStamp")

    // Render stamp message.
    MObject defaultString = stringDataFn.create("appleseed {lib-version} | Time: {render-time}");
    m_renderStampString = typedAttrFn.create("renderStampString", "renderStampString", MFnData::kString, defaultString, &status);
    CHECKED_ADD_ATTRIBUTE(m_renderStampString, "m_renderStampString")

    // Log level.
    const short defaultLogLevel = static_cast<short>(asf::LogMessage::Info);
    m_logLevel = enumAttrFn.create("logLevel", "logLevel", defaultLogLevel, &status);
    enumAttrFn.addField("Debug", 0);
    enumAttrFn.addField("Info", 1);
    enumAttrFn.addField("Warning", 2);
    enumAttrFn.addField("Error", 3);
    enumAttrFn.addField("Fatal", 4);
    CHECKED_ADD_ATTRIBUTE(m_logLevel, "logLevel")

    // Log filename.
    m_logFilename = typedAttrFn.create("logFilename", "logFilename", MFnData::kString, &status);
    typedAttrFn.setUsedAsFilename(true);
    CHECKED_ADD_ATTRIBUTE(m_logFilename, "logFilename")

    #undef CHECKED_ADD_ATTRIBUTE

    return status;
}

MStatus RenderGlobalsNode::compute(const MPlug& plug, MDataBlock& dataBlock)
{
    return MS::kSuccess;
}

void RenderGlobalsNode::applyGlobalsToProject(
    const MObject&                              globals,
    AppleseedSession::SessionMode               sessionMode,
    asr::Project&                               project,
    asr::AOVContainer&                          aovs)
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

    int passes;
    if (AttributeUtils::get(MPlug(globals, m_passes), passes))
    {
        finalParams.insert_path("passes", passes);
        finalParams.insert_path("shading_result_framebuffer", passes == 1 ? "ephemeral" : "permanent");
    }

    // Adaptive tile sampler params.
    bool adaptiveSampling;
    if (AttributeUtils::get(MPlug(globals, m_adaptiveSampling), adaptiveSampling))
    {
        finalParams.insert_path(
            "tile_renderer",
            adaptiveSampling ? "adaptive" : "generic");
    }

    int minSamples;
    if (AttributeUtils::get(MPlug(globals, m_minPixelSamples), minSamples))
        finalParams.insert_path("adaptive_tile_renderer.min_samples", minSamples);

    int maxSamples;
    if (AttributeUtils::get(MPlug(globals, m_maxPixelSamples), maxSamples))
    {
        finalParams.insert_path("uniform_pixel_renderer.samples", maxSamples);
        finalParams.insert_path("adaptive_tile_renderer.max_samples", maxSamples);

        if (maxSamples == 1)
            finalParams.insert_path("uniform_pixel_renderer.force_antialiasing", true);
    }

    int batchSampleSize;
    if (AttributeUtils::get(MPlug(globals, m_batchSampleSize), batchSampleSize))
        finalParams.insert_path("adaptive_tile_renderer.batch_size", batchSampleSize);

    float sampleNoiseThreshold;
    if (AttributeUtils::get(MPlug(globals, m_sampleNoiseThreshold), sampleNoiseThreshold))
        finalParams.insert_path("adaptive_tile_renderer.noise_threshold", sampleNoiseThreshold);

    asr::Frame* frame = project.get_frame();

    int tileSize;
    if (AttributeUtils::get(MPlug(globals, m_tileSize), tileSize))
    {
        frame->get_parameters().insert(
            "tile_size", asf::Vector2i(tileSize));
    }

    // Pixel filter params.
    int pixelFilter;
    if (AttributeUtils::get(MPlug(globals, m_pixelFilter), pixelFilter))
        frame->get_parameters().insert("filter", m_pixelFilterKeys[pixelFilter].asChar());

    float pixelFilterSize;
    if (AttributeUtils::get(MPlug(globals, m_pixelFilterSize), pixelFilterSize))
        frame->get_parameters().insert("filter_size", pixelFilterSize);

    // Noise seed.
    int noiseSeed;
    if (AttributeUtils::get(MPlug(globals, m_noiseSeed), noiseSeed))
    {
        // Keeps noise seed constant for each frame.
        bool disableVaryNoiseSeed;
        if (AttributeUtils::get(MPlug(globals, m_lockSamplingPattern), disableVaryNoiseSeed) && disableVaryNoiseSeed)
        {
            frame->get_parameters().insert("noise_seed", noiseSeed);
        }
        else
        {
            // currentTime().value() returns the current animation frame as double.
            const int frameNumber = static_cast<int>(MAnimControl::currentTime().value());
            frame->get_parameters().insert("noise_seed", noiseSeed + frameNumber);
        }
    }

    // Shading overrides params.
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

    // Lighting params.
    int lightingEngine;
    if (AttributeUtils::get(MPlug(globals, m_lightingEngine), lightingEngine))
    {
        switch (lightingEngine)
        {
          case 0:     
            finalParams.insert_path("lighting_engine", "pt");
            break;
          case 1:
            finalParams.insert_path("lighting_engine", "sppm");
            break;
        }
    }

    // Light sampling algorithm params.
    int lightSamplingAlgorithm;
    if (AttributeUtils::get(MPlug(globals, m_lightSamplingAlgorithm), lightSamplingAlgorithm))
    {
        switch (lightSamplingAlgorithm)
        {
          case 0:
            INSERT_PATH_IN_CONFIGS("light_sampler.algorithm", "cdf");
            break;
          case 1:
            INSERT_PATH_IN_CONFIGS("light_sampler.algorithm", "lighttree");
            break;
        }
    }

    // Light importance sampling params.
    bool enableLightImportanceSampling = false;
    if (AttributeUtils::get(MPlug(globals, m_enableLightImportanceSampling), enableLightImportanceSampling))
        INSERT_PATH_IN_CONFIGS("light_sampler.enable_importance_sampling", enableLightImportanceSampling);

    //
    // Path Tracing
    //

    // Direct lighting parameters.
    bool enableDL = true;
    if (AttributeUtils::get(MPlug(globals, m_enableDirectLighting), enableDL))
        INSERT_PATH_IN_CONFIGS("pt.enable_dl", enableDL);

    // Image-based lighting parameters.
    bool enableIBL = true;
    if (AttributeUtils::get(MPlug(globals, m_enableIBL), enableIBL))
        INSERT_PATH_IN_CONFIGS("pt.enable_ibl", enableIBL);

    // Bounce limits parameters.
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
        INSERT_PATH_IN_CONFIGS("pt.enable_caustics", caustics);

    bool enableMaxRayIntensity = false;
    AttributeUtils::get(MPlug(globals, m_enableMaxRayIntensity), enableMaxRayIntensity);

    if (enableMaxRayIntensity)
    {
       float maxRayIntensity;
       if (AttributeUtils::get(MPlug(globals, m_maxRayIntensity), maxRayIntensity))
            INSERT_PATH_IN_CONFIGS("pt.max_ray_intensity", maxRayIntensity);
    }

    bool clampRoughness;
    if (AttributeUtils::get(MPlug(globals, m_clampRoughness), clampRoughness))
        INSERT_PATH_IN_CONFIGS("pt.clamp_roughness", clampRoughness);

    float lightSamples;
    if (AttributeUtils::get(MPlug(globals, m_lightSamples), lightSamples))
        INSERT_PATH_IN_CONFIGS("pt.dl_light_samples", lightSamples);

    float envSamples;
    if (AttributeUtils::get(MPlug(globals, m_envSamples), envSamples))
        INSERT_PATH_IN_CONFIGS("pt.ibl_env_samples", envSamples);

    float lowLightThreshold = 0.0f;
    if (AttributeUtils::get(MPlug(globals, m_lowLightThreshold), lowLightThreshold))
        INSERT_PATH_IN_CONFIGS("pt.dl_low_light_threshold", lowLightThreshold);

    //
    // Stochastic Progressive Photon Mapping
    //

    // Photon type params.
    int sppmPhotonType;
    if (AttributeUtils::get(MPlug(globals, m_sppm_photon_type), sppmPhotonType))
    {
        switch (sppmPhotonType)
        {
          case 0:
            INSERT_PATH_IN_CONFIGS("sppm.photon_type", "poly");
            break;
          case 1:
            INSERT_PATH_IN_CONFIGS("sppm.photon_type", "mono");
            break;
        }
    }

    int sppmDirectLightingMode;
    if (AttributeUtils::get(MPlug(globals, m_sppm_direct_lighting_mode), sppmDirectLightingMode))
    {
        switch (sppmDirectLightingMode)
        {
          case 0:
            INSERT_PATH_IN_CONFIGS("sppm.dl_mode", "rt");
            break;
          case 1:
            INSERT_PATH_IN_CONFIGS("sppm.dl_mode", "sppm");
            break;
          case 2:
            INSERT_PATH_IN_CONFIGS("sppm.dl_mode", "off");
            break;
        }
    }

    bool SPPMCaustics;
    if (AttributeUtils::get(MPlug(globals, m_sppm_enable_caustics), SPPMCaustics))
        INSERT_PATH_IN_CONFIGS("sppm.enable_caustics", SPPMCaustics);

    bool SPPMEnableIBL;
    if (AttributeUtils::get(MPlug(globals, m_sppm_enable_ibl), SPPMEnableIBL))
        INSERT_PATH_IN_CONFIGS("sppm.enable_ibl", SPPMEnableIBL);

    bool limitPhotonTracingBounces = false;
    AttributeUtils::get(MPlug(globals, m_sppm_photon_tracing_enable_bounce_limit), limitPhotonTracingBounces);

    if (limitPhotonTracingBounces)
    {
        int photon_bounces = -1;
        if (AttributeUtils::get(MPlug(globals, m_sppm_photon_tracing_max_bounces), photon_bounces))
            INSERT_PATH_IN_CONFIGS("sppm.photon_tracing_max_bounces", photon_bounces);
    }

    int photon_tracing_rr_start_bounce;
    if (AttributeUtils::get(MPlug(globals, m_sppm_photon_tracing_rr_min_path_length), photon_tracing_rr_start_bounce))
        INSERT_PATH_IN_CONFIGS("sppm.photon_tracing_rr_min_path_length", photon_tracing_rr_start_bounce);

    int photon_tracing_light_photons;
    if (AttributeUtils::get(MPlug(globals, m_sppm_photon_tracing_light_photons), photon_tracing_light_photons))
        INSERT_PATH_IN_CONFIGS("sppm.light_photons_per_pass", photon_tracing_light_photons);

    int photon_tracing_environment_photons;
    if (AttributeUtils::get(MPlug(globals, m_sppm_photon_tracing_environment_photons), photon_tracing_environment_photons))
        INSERT_PATH_IN_CONFIGS("sppm.env_photons_per_pass", photon_tracing_environment_photons);

    bool limitRadianceEstimationBounces = false;
    AttributeUtils::get(MPlug(globals, m_sppm_radiance_estimation_enable_bounce_limit), limitRadianceEstimationBounces);

    if (limitRadianceEstimationBounces)
    {
        int radiance_estimation_max_bounces = -1;
        if (AttributeUtils::get(MPlug(globals, m_sppm_radiance_estimation_max_bounces), radiance_estimation_max_bounces))
            INSERT_PATH_IN_CONFIGS("sppm.path_tracing_max_bounces", radiance_estimation_max_bounces);
    }

    int radiance_estimation_rr_start_bounce;
    if (AttributeUtils::get(MPlug(globals, m_sppm_radiance_estimation_rr_min_path_length), radiance_estimation_rr_start_bounce))
        INSERT_PATH_IN_CONFIGS("sppm.path_tracing_rr_min_path_length", radiance_estimation_rr_start_bounce);

    float radiance_estimation_initial_radius;
    if (AttributeUtils::get(MPlug(globals, m_sppm_radiance_estimation_initial_radius), radiance_estimation_initial_radius))
        INSERT_PATH_IN_CONFIGS("sppm.initial_radius", radiance_estimation_initial_radius);

    int radiance_estimation_max_photons;
    if (AttributeUtils::get(MPlug(globals, m_sppm_radiance_estimation_max_photons), radiance_estimation_max_photons))
        INSERT_PATH_IN_CONFIGS("sppm.max_photons_per_estimate", radiance_estimation_max_photons);

    float radiance_estimation_alpha;
    if (AttributeUtils::get(MPlug(globals, m_sppm_radiance_estimation_alpha), radiance_estimation_alpha))
        INSERT_PATH_IN_CONFIGS("sppm.alpha", radiance_estimation_alpha);

    bool enableMaxRayIntensitySPPM = false;
    AttributeUtils::get(MPlug(globals, m_sppm_max_ray_intensity_set), enableMaxRayIntensitySPPM);

    if (enableMaxRayIntensitySPPM)
    {
        float sppm_max_ray_int;
        if (AttributeUtils::get(MPlug(globals, m_sppm_max_ray_intensity), sppm_max_ray_int))
            INSERT_PATH_IN_CONFIGS("sppm.path_tracing_max_ray_intensity", sppm_max_ray_int);
    }

    // Sytem params.
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

    bool useEmbree;
    if (AttributeUtils::get(MPlug(globals, m_useEmbree), useEmbree))
        INSERT_PATH_IN_CONFIGS("use_embree", useEmbree)

    // Denoiser params.
    int denoiserMode;
    if (AttributeUtils::get(MPlug(globals, m_denoiserMode), denoiserMode))
    {
        frame->get_parameters().insert(
            "denoiser", m_denoiserModeKeys[denoiserMode].asChar());
    }

    bool skipDenoised;
    if (AttributeUtils::get(MPlug(globals, m_skipDenoisedPixels), skipDenoised))
    {
        frame->get_parameters().insert(
            "skip_denoised", skipDenoised);
    }

    bool randomPixelOrder;
    if (AttributeUtils::get(MPlug(globals, m_randomPixelOrder), randomPixelOrder))
    {
        frame->get_parameters().insert(
            "random_pixel_order", randomPixelOrder);
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

    // AOVs.
    if (sessionMode != AppleseedSession::ProgressiveRenderSession)
    {
        asr::AOVFactoryRegistrar registrar;
        asr::ParamArray params;

        bool enabled;

        if (AttributeUtils::get(MPlug(globals, m_albedoAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("albedo_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_cryptomatteMaterialAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("cryptomatte_material_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_cryptomatteObjectAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("cryptomatte_object_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_depthAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("depth_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_diffuseAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("diffuse_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_directDiffuseAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("direct_diffuse_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_directGlossyAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("direct_glossy_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_emissionAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("emission_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_glossyAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("glossy_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_indirectDiffuseAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("indirect_diffuse_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_indirectGlossyAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("indirect_glossy_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_invalidSamplesAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("invalid_samples_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_normalAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("normal_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_nprContourAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("npr_contour_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_nprShadingAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("npr_shading_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_pixelErrorAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("pixel_error_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_pixelSampleCountAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("pixel_sample_count_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_pixelTimeAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("pixel_time_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_pixelVariationAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("pixel_variation_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_positionAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("position_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_uvAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("uv_aov")->create(params));

        if (AttributeUtils::get(MPlug(globals, m_velocityAOV), enabled))
            if (enabled) aovs.insert(registrar.lookup("screen_space_velocity_aov")->create(params));

    }

    #undef INSERT_PATH_IN_CONFIGS
#undef REMOVE_PATH_IN_CONFIGS
}

// Post-processing.
void RenderGlobalsNode::applyPostProcessStagesToFrame(const MObject& globals, asr::Project& project)
{
    bool enabled;
    if (AttributeUtils::get(MPlug(globals, m_renderStamp), enabled))
    {
        if (enabled)
        {
            MString string;
            if (AttributeUtils::get(MPlug(globals, m_renderStampString), string))
            {
                asr::Frame* frame = project.get_frame();

                frame->post_processing_stages().insert(
                    asr::RenderStampPostProcessingStageFactory().create(
                        "render_stamp",
                        asr::ParamArray()
                            .insert("order", 0)
                            .insert("format_string", string.asChar())));
            }
        }
    }
}

// Motion blur.
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
                static_cast<size_t>(cameraSamples),
                motionBlurSampleTimes.m_shutterOpenTime,
                motionBlurSampleTimes.m_shutterCloseTime,
                motionBlurSampleTimes.m_cameraTimes);
        }

        int xformSamples = 1;
        if (AttributeUtils::get(MPlug(globals, m_mbTransformSamples), xformSamples))
        {
            motionBlurSampleTimes.initializeFrameSet(
                static_cast<size_t>(xformSamples),
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
                static_cast<size_t>(deformSamples),
                motionBlurSampleTimes.m_shutterOpenTime,
                motionBlurSampleTimes.m_shutterCloseTime,
                motionBlurSampleTimes.m_deformTimes);
        }

        motionBlurSampleTimes.mergeTimes();
    }
    else
        motionBlurSampleTimes.initializeToCurrentFrame();
}

// Render log.
asf::LogMessage::Category RenderGlobalsNode::logLevel(const MObject& globals)
{
    short level = static_cast<short>(asf::LogMessage::Warning);
    AttributeUtils::get(MPlug(globals, m_logLevel), level);
    return static_cast<asf::LogMessage::Category>(level);
}

MString RenderGlobalsNode::logFilename(const MObject& globals)
{
    MString filename;
    AttributeUtils::get(MPlug(globals, m_logFilename), filename);
    return filename;
}
