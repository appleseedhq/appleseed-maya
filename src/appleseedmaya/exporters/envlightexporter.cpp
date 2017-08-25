
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
#include "envlightexporter.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exporters/exporterfactory.h"
#include "appleseedmaya/physicalskylightnode.h"
#include "appleseedmaya/skydomelightnode.h"

// appleseed.renderer headers.
#include "renderer/api/environmentedf.h"
#include "renderer/api/scene.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MFnDagNode.h>
#include "appleseedmaya/_endmayaheaders.h"

namespace asf = foundation;
namespace asr = renderer;

EnvLightExporter::EnvLightExporter(
    const MDagPath&                             path,
    asr::Project&                               project,
    AppleseedSession::SessionMode               sessionMode)
  : DagNodeExporter(path, project, sessionMode)
{
}

EnvLightExporter::~EnvLightExporter()
{
    if (sessionMode() == AppleseedSession::ProgressiveRenderSession)
    {
        scene().environment_shaders().remove(m_envShader.get());
        scene().environment_edfs().remove(m_envLight.get());
    }
}

void EnvLightExporter::createEntities(
    const AppleseedSession::Options&            options,
    const AppleseedSession::MotionBlurTimes&    motionBlurTimes)
{
    asr::EnvironmentShaderFactoryRegistrar factoryRegistrar;
    const asr::IEnvironmentShaderFactory* factory = factoryRegistrar.lookup("edf_environment_shader");

    MString envShaderName = appleseedName() + "_shader";
    m_envShader = factory->create(
        envShaderName.asChar(),
        asr::ParamArray().insert(
            "environment_edf",
            appleseedName().asChar()));
}

void EnvLightExporter::flushEntities()
{
    if (m_envLight.get())
        scene().environment_edfs().insert(m_envLight.release());

    if (m_envShader.get())
        scene().environment_shaders().insert(m_envShader.release());
}

void PhysicalSkyLightExporter::registerExporter()
{
    NodeExporterFactory::registerDagNodeExporter(
        PhysicalSkyLightNode::nodeName,
        &PhysicalSkyLightExporter::create);
}

DagNodeExporter* PhysicalSkyLightExporter::create(
    const MDagPath&                             path,
    asr::Project&                               project,
    AppleseedSession::SessionMode               sessionMode)
{
    if (areObjectAndParentsRenderable(path) == false)
        return nullptr;

    return new PhysicalSkyLightExporter(path, project, sessionMode);
}

PhysicalSkyLightExporter::PhysicalSkyLightExporter(
    const MDagPath&                             path,
    asr::Project&                               project,
    AppleseedSession::SessionMode               sessionMode)
  : EnvLightExporter(path, project,             sessionMode)
{
}

PhysicalSkyLightExporter::~PhysicalSkyLightExporter()
{
    if (sessionMode() == AppleseedSession::ProgressiveRenderSession)
    {
        if (m_sunLight.get())
            mainAssembly().lights().remove(m_sunLight.get());
    }
}

void PhysicalSkyLightExporter::createEntities(
    const AppleseedSession::Options&            options,
    const AppleseedSession::MotionBlurTimes&    motionBlurTimes)
{
    asr::ParamArray params;
    MAngle angle;
    float val;

    AttributeUtils::get(node(), "sunTheta", angle);
    params.insert("sun_theta", angle.asDegrees());

    AttributeUtils::get(node(), "sunPhi", angle);
    params.insert("sun_phi", angle.asDegrees());

    AttributeUtils::get(node(), "turbidityScale", val);
    params.insert("turbidity_multiplier", val);

    AttributeUtils::get(node(), "luminanceScale", val);
    params.insert("luminance_multiplier", val);

    AttributeUtils::get(node(), "luminanceGamma", val);
    params.insert("luminance_gamma", val);

    AttributeUtils::get(node(), "saturationScale", val);
    params.insert("saturation_multiplier", val);

    AttributeUtils::get(node(), "horizonShift", angle);
    params.insert("horizon_shift", angle.asDegrees());

    AttributeUtils::get(node(), "groundAlbedo", val);
    params.insert("ground_albedo", val);

    float turbidity = 4.0f;
    AttributeUtils::get(node(), "turbidity", turbidity);
    params.insert("turbidity", turbidity);

    m_envLight = asr::HosekEnvironmentEDFFactory().create(
        appleseedName().asChar(),
        params);

    EnvLightExporter::createEntities(options, motionBlurTimes);

    bool sunEnabled = false;
    AttributeUtils::get(node(), "sunEnable", sunEnabled);
    if (sunEnabled)
    {
        float radianceScale = 1.0f;
        AttributeUtils::get(node(), "sunRadianceScale", radianceScale);

        float sizeScale = 1.0f;
        AttributeUtils::get(node(), "sunSizeScale", sizeScale);

        MString name = appleseedName();
        name += "_sun_light";

        m_sunLight = asr::SunLightFactory().create(
            name.asChar(),
            asr::ParamArray()
                .insert("environment_edf", appleseedName().asChar())
                .insert("turbidity", turbidity)
                .insert("radiance_multiplier", radianceScale)
                .insert("size_multiplier", sizeScale));
    }
}

void PhysicalSkyLightExporter::flushEntities()
{
    EnvLightExporter::flushEntities();

    if (m_sunLight.get())
        mainAssembly().lights().insert(m_sunLight.release());
}

void SkyDomeLightExporter::registerExporter()
{
    NodeExporterFactory::registerDagNodeExporter(
        SkyDomeLightNode::nodeName,
        &SkyDomeLightExporter::create);
}

DagNodeExporter* SkyDomeLightExporter::create(
    const MDagPath&                             path,
    asr::Project&                               project,
    AppleseedSession::SessionMode               sessionMode)
{
    if (areObjectAndParentsRenderable(path) == false)
        return nullptr;

    return new SkyDomeLightExporter(path, project, sessionMode);
}

SkyDomeLightExporter::SkyDomeLightExporter(
    const MDagPath&                             path,
    asr::Project&                               project,
    AppleseedSession::SessionMode               sessionMode)
  : EnvLightExporter(path, project,             sessionMode)
{
}

SkyDomeLightExporter::~SkyDomeLightExporter()
{
    if (sessionMode() == AppleseedSession::ProgressiveRenderSession)
    {
        if (m_mapTexture.get())
            scene().textures().remove(m_mapTexture.get());

        if (m_mapTextureInstance.get())
            scene().texture_instances().remove(m_mapTextureInstance.get());
    }
}

void SkyDomeLightExporter::createEntities(
    const AppleseedSession::Options&            options,
    const AppleseedSession::MotionBlurTimes&    motionBlurTimes)
{
    asr::ParamArray params;

    MString map;
    float val;
    MAngle angle;

    AttributeUtils::get(node(), "map", map);
    MString textureName = appleseedName() + "_texture";
    m_mapTexture = asr::DiskTexture2dFactory().create(
        textureName.asChar(),
        asr::ParamArray()
            .insert("filename", map.asChar())
            .insert("color_space", "linear_rgb"),
        project().search_paths());

    MString textureInstanceName = textureName + "_instance";
    m_mapTextureInstance = asr::TextureInstanceFactory().create(
        textureInstanceName.asChar(),
        asr::ParamArray(),
        textureName.asChar());
    params.insert("radiance", textureInstanceName.asChar());

    AttributeUtils::get(node(), "intensity", val);
    params.insert("radiance_multiplier", val);

    AttributeUtils::get(node(), "exposure", val);
    params.insert("exposure", val);

    AttributeUtils::get(node(), "hShift", angle);
    params.insert("horizontal_shift", angle.asDegrees());

    AttributeUtils::get(node(), "vShift", angle);
    params.insert("vertical_shift", angle.asDegrees());

    m_envLight = asr::LatLongMapEnvironmentEDFFactory().create(
        appleseedName().asChar(),
        params);

    EnvLightExporter::createEntities(options, motionBlurTimes);
}

void SkyDomeLightExporter::exportTransformMotionStep(float time)
{
    asf::Matrix4d m = convert(dagPath().inclusiveMatrix());
    asf::Matrix4d invM = convert(dagPath().inclusiveMatrixInverse());
    asf::Transformd xform(m, invM);
    m_envLight->transform_sequence().set_transform(time, xform);
}

void SkyDomeLightExporter::flushEntities()
{
    if (m_mapTexture.get())
        scene().textures().insert(m_mapTexture.release());

    if (m_mapTextureInstance.get())
        scene().texture_instances().insert(m_mapTextureInstance.release());

    EnvLightExporter::flushEntities();
}
