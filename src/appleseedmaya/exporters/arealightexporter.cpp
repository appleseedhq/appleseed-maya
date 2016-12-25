
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016 Esteban Tovagliari, The appleseedhq Organization
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
#include "appleseedmaya/exporters/arealightexporter.h"

// Maya headers.
#include <maya/MFnDagNode.h>

// appleseed.foundation headers.
#include "foundation/math/scalar.h"

// appleseed.renderer headers.
#include "renderer/api/scene.h"

// appleseed.maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exporters/exporterfactory.h"
#include "appleseedmaya/exporters/shadingnetworkexporter.h"

namespace asf = foundation;
namespace asr = renderer;

void AreaLightExporter::registerExporter()
{
    NodeExporterFactory::registerDagNodeExporter("areaLight", &AreaLightExporter::create);
}

DagNodeExporter *AreaLightExporter::create(
    const MDagPath&                 path,
    asr::Project&                   project,
    AppleseedSession::SessionMode   sessionMode)
{
    return new AreaLightExporter(path, project, sessionMode);
}

AreaLightExporter::AreaLightExporter(
    const MDagPath&                 path,
    asr::Project&                   project,
    AppleseedSession::SessionMode   sessionMode)
  : DagNodeExporter(path, project, sessionMode)
{
}

AreaLightExporter::~AreaLightExporter()
{
    if (sessionMode() == AppleseedSession::ProgressiveRenderSession)
    {
        // todo: cleanup here.
    }
}

bool AreaLightExporter::supportsMotionBlur() const
{
    return false;
}

void AreaLightExporter::createExporters(const AppleseedSession::Services& services)
{
    m_lightNetworkExporter = services.createShadingNetworkExporter(
        AreaLightNetworkContext,
        node(),
        MPlug());
}

void AreaLightExporter::createEntity(const AppleseedSession::Options& options)
{
    MString objectName = appleseedName();

    asr::ParamArray params;
    params.insert("primitive", "grid");
    params.insert("resolution_u", 1);
    params.insert("resolution_v", 1);
    params.insert("width", 2.0f);
    params.insert("height", 2.0f);

    if (sessionMode() == AppleseedSession::ExportSession)
        m_lightMesh.reset(asr::MeshObjectFactory::create(objectName.asChar(), params));
    else
        m_lightMesh.reset(asr::create_primitive_mesh(objectName.asChar(), params));

    MString materialName = objectName + MString("_area_light_material");
    m_material.reset(asr::OSLMaterialFactory().create(materialName.asChar(), asr::ParamArray()));

    asf::StringDictionary materialMappings;
    materialMappings.insert("default", materialName.asChar());

    asf::Matrix4d m = convert(dagPath().inclusiveMatrix());

    // Rotate x -90 degrees (or is it 90?) here.
    m = m * asf::Matrix4d::make_rotation_x(asf::deg_to_rad(-90.0));
    asf::Transformd xform(m, asf::inverse(m));

    const MString objectInstanceName = appleseedName() + MString("_instance");
    params.clear();
    visibilityAttributesToParams(params);
    m_objectInstance.reset(
        asr::ObjectInstanceFactory::create(
            objectInstanceName.asChar(),
            params,
            objectName.asChar(),
            xform,
            materialMappings,
            materialMappings));
}

void AreaLightExporter::flushEntity()
{
    if (m_lightNetworkExporter)
    {
        m_material->get_parameters().insert(
            "osl_surface",
            m_lightNetworkExporter->shaderGroupName().asChar());
    }

    mainAssembly().materials().insert(m_material.release());
    mainAssembly().objects().insert(m_lightMesh.releaseAs<asr::Object>());
    mainAssembly().object_instances().insert(m_objectInstance.release());
}
