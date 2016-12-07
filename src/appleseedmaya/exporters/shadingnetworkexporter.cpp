
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
#include "appleseedmaya/exporters/shadingnetworkexporter.h"

// Standard headers.
#include <algorithm>

// Maya headers.
#include <maya/MItDependencyGraph.h>
#include <maya/MFnDependencyNode.h>

// appleseed.renderer headers.
#include "renderer/api/scene.h"

// appleseed.maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exporters/exporterfactory.h"
#include "appleseedmaya/exporters/shadingnodeexporter.h"
#include "appleseedmaya/logger.h"
#include "appleseedmaya/shadingnoderegistry.h"

namespace asf = foundation;
namespace asr = renderer;

ShadingNetworkExporter::ShadingNetworkExporter(
    const ShadingNetworkContext   context,
    const MObject&                object,
    const MPlug&                  outputPlug,
    renderer::Assembly&           mainAssembly,
    AppleseedSession::SessionMode sessionMode)
  : m_context(context)
  , m_object(object)
  , m_outputPlug(outputPlug)
  , m_mainAssembly(mainAssembly)
  , m_sessionMode(sessionMode)
{
}

MString ShadingNetworkExporter::shaderGroupName() const
{
    return m_shaderGroup->get_name();
}

void ShadingNetworkExporter::createEntity(const AppleseedSession::Options& options)
{
    MFnDependencyNode depNodeFn(m_object);
    MString shaderGroupName = depNodeFn.name() + MString("_shader_group");
    m_shaderGroup = asr::ShaderGroupFactory::create(shaderGroupName.asChar());
    exportShadingNetwork();
}

void ShadingNetworkExporter::flushEntity()
{
    insertEntityWithUniqueName(
        m_mainAssembly.shader_groups(),
        m_shaderGroup);
}

void ShadingNetworkExporter::exportShadingNetwork()
{
    MStatus status;

    m_shaderGroup->clear();
    m_nodeExporters.clear();
    m_nodesCreated.clear();

    createShaderNodeExporters(m_object);
    std::reverse(m_nodeExporters.begin(), m_nodeExporters.end());

    for(size_t i = 0, e = m_nodeExporters.size(); i < e; ++i)
        m_nodeExporters[i]->createShader();

    switch(m_context)
    {
        case SurfaceNetworkContext:
        {
            // Create the shader to surface adaptor.
            m_shaderGroup->add_shader(
                "surface",
                "as_maya_closure2Surface",
                "closureToSurface",
                asr::ParamArray());

            // Connect the shader to the surface adaptor.
            MFnDependencyNode depNodeFn(m_object);
            const OSLShaderInfo *shaderInfo = ShadingNodeRegistry::getShaderInfo(depNodeFn.typeName());
            if(shaderInfo)
            {
                if(const OSLParamInfo *srcParamInfo = shaderInfo->findParam(m_outputPlug))
                {
                    m_shaderGroup->add_connection(
                        depNodeFn.name().asChar(),
                        srcParamInfo->paramName.asChar(),
                        "closureToSurface",
                        "in_input");
                }
                else
                {
                    MStatus status;
                    const MString attrName =
                        m_outputPlug.partialName(
                            false,
                            false,
                            false,
                            false,
                            false,
                            true,   // use long names.
                            &status);
                    RENDERER_LOG_DEBUG(
                        "Skipping unknown attribute %s of shading node %s",
                        attrName.asChar(),
                        depNodeFn.typeName().asChar());
                }
            }
        }
        break;

        default:
            assert(false);
            RENDERER_LOG_ERROR("Unknown shading network context.");
        break;
    }
}

void ShadingNetworkExporter::createShaderNodeExporters(const MObject& node)
{
    MStatus status;
    MFnDependencyNode depNodeFn(node);

    if(m_nodesCreated.count(depNodeFn.name()) != 0)
    {
        RENDERER_LOG_DEBUG(
            "Skipping already exported shading node %s.",
            depNodeFn.name().asChar());
        return;
    }

    const OSLShaderInfo *shaderInfo = ShadingNodeRegistry::getShaderInfo(depNodeFn.typeName());
    if(shaderInfo)
    {
        ShadingNodeExporterPtr exporter(
            NodeExporterFactory::createShadingNodeExporter(
                node,
                *m_shaderGroup));
        m_nodeExporters.push_back(exporter);
        m_nodesCreated.insert(depNodeFn.name());
        RENDERER_LOG_DEBUG("Created shading node exporter for node %s", depNodeFn.name().asChar());

        for(int i = 0, e = shaderInfo->paramInfo.size(); i < e; ++i)
        {
            const OSLParamInfo& paramInfo = shaderInfo->paramInfo[i];

            // Skip output attributes.
            if(paramInfo.isOutput)
                continue;

            MPlug plug = depNodeFn.findPlug(paramInfo.mayaAttributeName, &status);
            if(!status)
            {
                RENDERER_LOG_WARNING(
                    "Skipping unknown attribute %s of shading node %s",
                    paramInfo.mayaAttributeName.asChar(),
                    depNodeFn.typeName().asChar());
                continue;
            }

            if(plug.isConnected())
            {
                MObject srcNode;
                if(AttributeUtils::get(plug, srcNode))
                    createShaderNodeExporters(srcNode);
                continue;
            }

            if(plug.isCompound() && plug.numConnectedChildren() != 0)
            {
                for(size_t i = 0, e = plug.numChildren(); i < e; ++i)
                {
                    MPlug childPlug = plug.child(i, &status);
                    if(status)
                    {
                        MObject srcNode;
                        if(AttributeUtils::get(childPlug, srcNode))
                            createShaderNodeExporters(srcNode);
                    }
                }
                continue;
            }

            if(plug.isArray() && plug.numConnectedElements() != 0)
            {
                for(size_t i = 0, e = plug.numElements(); i < e; ++i)
                {
                    MPlug elementPlug = plug.elementByLogicalIndex(i, &status);
                    if(status)
                    {
                        MObject srcNode;
                        if(AttributeUtils::get(elementPlug, srcNode))
                            createShaderNodeExporters(srcNode);
                    }
                }
            }
        }
    }
    else
    {
        RENDERER_LOG_WARNING(
            "Found unsupported shading node %s while exporting network",
            depNodeFn.typeName().asChar());
    }
}
