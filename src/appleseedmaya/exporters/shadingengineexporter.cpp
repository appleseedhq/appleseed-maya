
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
#include "renderer/api/project.h"
#include "renderer/api/scene.h"

namespace asf = foundation;
namespace asr = renderer;

ShadingEngineExporter::ShadingEngineExporter(
    const MObject&                  object,
    asr::Project&                   project)
  : m_object(object)
  , m_project(project)
  , m_scene(*project.get_scene())
  , m_mainAssembly(*m_scene.assemblies().get_by_name("assembly"))
{
}

void ShadingEngineExporter::createEntity(const AppleseedSession::Options& options)
{
    MFnDependencyNode depNodeFn(m_object);
    const MString appleseedName = depNodeFn.name();

    MString surfaceShaderName = appleseedName + MString("_surface_shader");
    asr::ParamArray params;
    m_surfaceShader.reset(
        asr::PhysicalSurfaceShaderFactory().create(
            surfaceShaderName.asChar(),
            params));
    params.clear();

    MString materialName = appleseedName + MString("_material");
    params.insert("surface_shader", surfaceShaderName.asChar());
    m_material.reset(
        asr::OSLMaterialFactory().create(materialName.asChar(), params));
}

void ShadingEngineExporter::flushEntity()
{
    /*
    if(m_surfaceExporter)
    {
        m_surfaceExporter->flushEntity();
        m_material->get_parameters().insert(
            "osl_surface",
            m_surfaceExporter->shaderGroupName().asChar());
    }
    */

    m_mainAssembly.materials().insert(m_material.release());
    m_mainAssembly.surface_shaders().insert(m_surfaceShader.release());
}

/*
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
*/

/*
void ShadingEngineExporter::createShadingNetworkExporters(
    const AppleseedSession::Options& options)
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
            MFnDependencyNode otherDepNodeFn(otherNode);

            if(ShadingNodeRegistry::isShaderSupported(otherDepNodeFn.typeName()))
            {
                m_surfaceExporter.reset(
                    new ShadingNetworkExporter(
                        ShadingNetworkExporter::SurfaceContext,
                        otherNode,
                        project(),
                        sessionMode()));
                m_surfaceExporter->createEntity(options);
            }
            else
            {
                RENDERER_LOG_WARNING(
                    "Unsupported shading node type %s found",
                    otherDepNodeFn.typeName().asChar());
            }
        }
    }
}
*/
