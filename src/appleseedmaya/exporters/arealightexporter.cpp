
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
#include "arealightexporter.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exporters/exporterfactory.h"
#include "appleseedmaya/exporters/shadingnetworkexporter.h"

// appleseed.renderer headers.
#include "renderer/api/scene.h"

// appleseed.foundation headers.
#include "foundation/math/scalar.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MFnDagNode.h>
#include "appleseedmaya/_endmayaheaders.h"

namespace asf = foundation;
namespace asr = renderer;

void AreaLightExporter::registerExporter()
{
    NodeExporterFactory::registerDagNodeExporter("areaLight", &AreaLightExporter::create);
}

DagNodeExporter* AreaLightExporter::create(
    const MDagPath&                                 path,
    asr::Project&                                   project,
    AppleseedSession::SessionMode                   sessionMode)
{
    if (areObjectAndParentsRenderable(path) == false)
        return nullptr;

    return new AreaLightExporter(path, project, sessionMode);
}

AreaLightExporter::AreaLightExporter(
    const MDagPath&                                 path,
    asr::Project&                                   project,
    AppleseedSession::SessionMode                   sessionMode)
  : DagNodeExporter(path, project, sessionMode)
{
}

AreaLightExporter::~AreaLightExporter()
{
    if (sessionMode() == AppleseedSession::ProgressiveRenderSession)
    {
        mainAssembly().materials().remove(m_material.get());
        mainAssembly().materials().remove(m_backMaterial.get());
        mainAssembly().objects().remove(m_lightMesh.get());
        mainAssembly().object_instances().remove(m_objectInstance.get());
    }
}

bool AreaLightExporter::supportsMotionBlur() const
{
    return false;
}

void AreaLightExporter::createExporters(const AppleseedSession::IExporterFactory& exporter_factory)
{
    m_lightNetworkExporter = exporter_factory.createShadingNetworkExporter(
        AreaLightNetworkContext,
        node(),
        MPlug());
}

void AreaLightExporter::createEntities(
    const AppleseedSession::Options&                options,
    const AppleseedSession::MotionBlurSampleTimes&  motionBlurSampleTimes)
{
    MString objectName = appleseedName();

    asr::ParamArray params;
    params.insert("primitive", "grid");
    params.insert("resolution_u", 1);
    params.insert("resolution_v", 1);
    params.insert("width", 2.0f);
    params.insert("height", 2.0f);

    if (sessionMode() == AppleseedSession::ExportSession)
        m_lightMesh.reset(asr::MeshObjectFactory().create(objectName.asChar(), params));
    else
        m_lightMesh.reset(asr::create_primitive_mesh(objectName.asChar(), params));

    MString materialName = objectName + MString("_area_light_material");
    m_material.reset(asr::OSLMaterialFactory().create(materialName.asChar(), asr::ParamArray()));

    asf::StringDictionary frontMaterialMappings;
    frontMaterialMappings.insert("default", materialName.asChar());

    MString backMaterialName = objectName + MString("_area_light_back_material");
    m_backMaterial.reset(asr::GenericMaterialFactory().create(backMaterialName.asChar(), asr::ParamArray()));

    asf::StringDictionary backMaterialMappings;
    backMaterialMappings.insert("default", backMaterialName.asChar());

    asf::Matrix4d m = convert(dagPath().inclusiveMatrix());

    // Rotate to match Maya's default light orientation and UVs.
    m = m * asf::Matrix4d::make_rotation_x(asf::deg_to_rad(-90.0));
    m = m * asf::Matrix4d::make_rotation_y(asf::deg_to_rad(180.0));
    params.clear();

    const MString objectInstanceName = appleseedName() + MString("_instance");
    addVisibilityAttributesToParams(params);
    m_objectInstance.reset(
        asr::ObjectInstanceFactory::create(
            objectInstanceName.asChar(),
            params,
            objectName.asChar(),
            asf::Transformd(m),
            frontMaterialMappings,
            backMaterialMappings));
}

void AreaLightExporter::flushEntities()
{
    if (m_lightNetworkExporter)
    {
        m_material->get_parameters().insert(
            "osl_surface",
            m_lightNetworkExporter->shaderGroupName().asChar());
    }

    mainAssembly().materials().insert(m_material.release());
    mainAssembly().materials().insert(m_backMaterial.release());
    mainAssembly().objects().insert(m_lightMesh.release());
    mainAssembly().object_instances().insert(m_objectInstance.release());
}

asf::AABB3d AreaLightExporter::boundingBox() const
{
    asf::AABB3d bbox = objectSpaceBoundingBox(dagPath());
    const asf::Transformd xform(convert(dagPath().inclusiveMatrix()));
    return xform.to_parent(bbox);
}
