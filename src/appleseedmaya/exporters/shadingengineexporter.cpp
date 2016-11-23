
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
#include "appleseedmaya/exporters/shadingengineexporter.h"

// Maya headers.
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>

// appleseed.renderer headers.
#include "renderer/api/scene.h"

// appleseed.maya headers.
#include "appleseedmaya/exporters/exporterfactory.h"


namespace asf = foundation;
namespace asr = renderer;

void ShadingEngineExporter::registerExporter()
{
    NodeExporterFactory::registerMPxNodeExporter("shadingEngine", &ShadingEngineExporter::create);
}

MPxNodeExporter *ShadingEngineExporter::create(
    const MObject&                  object,
    asr::Project&                   project,
    AppleseedSession::SessionMode   sessionMode)
{
    return new ShadingEngineExporter(object, project, sessionMode);
}

ShadingEngineExporter::ShadingEngineExporter(
    const MObject&                  object,
    asr::Project&                   project,
    AppleseedSession::SessionMode   sessionMode)
  : MPxNodeExporter(object, project, sessionMode)
{
}

void ShadingEngineExporter::collectDependencyNodesToExport(MObjectArray& nodes)
{
    MFnDependencyNode depNodeFn(node());

    MStatus status;
    MPlug plug = depNodeFn.findPlug("surfaceShader", &status);

    if(plug.isConnected())
    {
        MPlugArray otherPlugs;
        plug.connectedTo(otherPlugs, true, false, &status);

        if(otherPlugs.length() == 1)
        {
            MObject otherNode = otherPlugs[0].node();
            nodes.append(otherPlugs[0].node());

            MFnDependencyNode otherDepNodeFn(otherNode);
            MString shaderGroupName = otherDepNodeFn.name() + MString("_shader_group");
            m_materialParams.insert(
                "osl_surface",
                shaderGroupName.asChar());
        }
    }
}

void ShadingEngineExporter::createEntity(const AppleseedSession::Options& options)
{
    MString surfaceShaderName = appleseedName() + MString("_surface_shader");
    m_surfaceShader.reset(
        asr::PhysicalSurfaceShaderFactory().create(
            surfaceShaderName.asChar(),
            asr::ParamArray()));

    MString materialName = appleseedName() + MString("_material");
    m_materialParams.insert("surface_shader", surfaceShaderName.asChar());
    m_material.reset(
        asr::OSLMaterialFactory().create(materialName.asChar(), m_materialParams));
}

void ShadingEngineExporter::flushEntity()
{
    mainAssembly().materials().insert(m_material.release());
    mainAssembly().surface_shaders().insert(m_surfaceShader.release());
}
