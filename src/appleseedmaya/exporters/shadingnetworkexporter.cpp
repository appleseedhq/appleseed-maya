
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
#include "shadingnetworkexporter.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exporters/exporterfactory.h"
#include "appleseedmaya/exporters/shadingnodeexporter.h"
#include "appleseedmaya/logger.h"
#include "appleseedmaya/shadingnodemetadata.h"
#include "appleseedmaya/shadingnoderegistry.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/scene.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MItDependencyGraph.h>
#include <maya/MFnDependencyNode.h>
#include "appleseedmaya/_endmayaheaders.h"

// Standard headers.
#include <algorithm>

namespace asf = foundation;
namespace asr = renderer;

namespace
{
    MStatus logUnknownAttributeFound(
        const MPlug&    outputPlug,
        const MString&  nodeTypeName)
    {
        MStatus status;
        const MString attrName =
            outputPlug.partialName(
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
            nodeTypeName.asChar());
        return status;
    }
}

ShadingNetworkExporter::ShadingNetworkExporter(
    const ShadingNetworkContext   context,
    const MObject&                object,
    const MPlug&                  outputPlug,
    renderer::Assembly&           mainAssembly,
    AppleseedSession::SessionMode sessionMode)
  : m_context(context)
  , m_sessionMode(sessionMode)
  , m_object(object)
  , m_outputPlug(outputPlug)
  , m_mainAssembly(mainAssembly)
{
}

ShadingNetworkExporter::~ShadingNetworkExporter()
{
    if (m_sessionMode == AppleseedSession::ProgressiveRenderSession)
        m_mainAssembly.shader_groups().remove(m_shaderGroup.get());
}

MString ShadingNetworkExporter::shaderGroupName() const
{
    assert(m_shaderGroup.get());
    return m_shaderGroup->get_name();
}

void ShadingNetworkExporter::createEntities()
{
    MFnDependencyNode depNodeFn(m_object);
    MString shaderGroupName = depNodeFn.name() + MString("_shader_group");
    m_shaderGroup = asr::ShaderGroupFactory::create(shaderGroupName.asChar());

    createShaderNodeExporters(m_object);

    // Create shader entities
    for (size_t i = 0, e = m_nodeExporters.size(); i < e; ++i)
        m_nodeExporters[i]->createEntities(m_namesToExporters);
}

void ShadingNetworkExporter::flushEntities()
{
    // Add any extra shader and or connections, depending on the context.
    switch (m_context)
    {
        case SurfaceNetworkContext:
        case SurfaceSwatchNetworkContext:
        {
            // Create the shader to surface adaptor.
            m_shaderGroup->add_shader(
                "surface",
                "as_closure2surface",
                "closureToSurface",
                asr::ParamArray());

            // Connect the shader to the surface adaptor.
            MFnDependencyNode depNodeFn(m_object);
            const OSLShaderInfo* shaderInfo = ShadingNodeRegistry::getShaderInfo(depNodeFn.typeName());
            if (shaderInfo)
            {
                if (const OSLParamInfo* srcParamInfo = shaderInfo->findParam(m_outputPlug))
                {
                    m_shaderGroup->add_connection(
                        depNodeFn.name().asChar(),
                        srcParamInfo->paramName.asChar(),
                        "closureToSurface",
                        "in_input");
                }
                else
                    logUnknownAttributeFound(m_outputPlug, depNodeFn.typeName());
            }
        }
        break;

        case AreaLightNetworkContext:
            // Nothing to do here...
        break;

        case TextureSwatchNetworkContext:
        {
            // Create the texture to surface adaptor.
            m_shaderGroup->add_shader(
                "surface",
                "as_texture2surface",
                "texture2Surface",
                asr::ParamArray());

            // Connect the texture to the surface adaptor.
            MFnDependencyNode depNodeFn(m_object);
            const OSLShaderInfo* shaderInfo = ShadingNodeRegistry::getShaderInfo(depNodeFn.typeName());
            if (shaderInfo)
            {
                if (const OSLParamInfo* srcParamInfo = shaderInfo->findParam(m_outputPlug))
                {
                    m_shaderGroup->add_connection(
                        depNodeFn.name().asChar(),
                        srcParamInfo->paramName.asChar(),
                        "texture2Surface",
                        srcParamInfo->paramType == "color"
                            ? "in_color"
                            : "in_scalar");
                }
                else
                    logUnknownAttributeFound(m_outputPlug, depNodeFn.typeName());
            }
        }
        break;

        default:
            assert(false);
        break;
    }

    insertEntityWithUniqueName(
        m_mainAssembly.shader_groups(),
        m_shaderGroup);
}

void ShadingNetworkExporter::createShaderNodeExporters(const MObject& node)
{
    MStatus status;
    MFnDependencyNode depNodeFn(node);
    if (m_namesToExporters.count(depNodeFn.name()) != 0)
    {
        RENDERER_LOG_DEBUG(
            "Skipping already exported shading node %s.",
            depNodeFn.name().asChar());
        return;
    }

    const OSLShaderInfo* shaderInfo = ShadingNodeRegistry::getShaderInfo(depNodeFn.typeName());
    if (shaderInfo)
    {
        // Look for nodes connected to the shader and create exporters for them.
        for (size_t i = 0, e = shaderInfo->paramInfo.size(); i < e; ++i)
        {
            const OSLParamInfo& paramInfo = shaderInfo->paramInfo[i];

            // Skip output attributes.
            if (paramInfo.isOutput)
                continue;

            MPlug plug = depNodeFn.findPlug(paramInfo.mayaAttributeName, /*wantNetworkedPlug=*/ false, &status);
            if (!status)
            {
                RENDERER_LOG_WARNING(
                    "Skipping unknown attribute %s of shading node %s",
                    paramInfo.mayaAttributeName.asChar(),
                    depNodeFn.typeName().asChar());
                continue;
            }

            if (plug.isConnected())
            {
                MPlug srcPlug;
                status = AttributeUtils::getPlugConnectedTo(plug, srcPlug);
                if (status)
                    createShaderNodeExporters(srcPlug.node());
            }

            // Look for nodes connected to child or elements plugs of this plug.
            if (plug.isCompound() && plug.numConnectedChildren() != 0)
            {
                for (unsigned int i = 0, e = plug.numChildren(); i < e; ++i)
                {
                    MPlug childPlug = plug.child(i, &status);
                    if (status)
                    {
                        MPlug srcPlug;
                        status = AttributeUtils::getPlugConnectedTo(childPlug, srcPlug);
                        if (status)
                            createShaderNodeExporters(srcPlug.node());
                    }
                }
            }
            else if (plug.isArray() && plug.numConnectedElements() != 0)
            {
                for (unsigned int i = 0, e = plug.numElements(); i < e; ++i)
                {
                    MPlug elementPlug = plug.elementByPhysicalIndex(i, &status);
                    if (status)
                    {
                        MPlug srcPlug;
                        status = AttributeUtils::getPlugConnectedTo(elementPlug, srcPlug);
                        if (status)
                            createShaderNodeExporters(srcPlug.node());
                    }
                }
            }
        }

        ShadingNodeExporterPtr exporter(
            NodeExporterFactory::createShadingNodeExporter(
                node,
                *m_shaderGroup));
        m_nodeExporters.push_back(exporter);
        m_namesToExporters[depNodeFn.name()] = exporter.get();
        RENDERER_LOG_DEBUG("Created shading node exporter for node %s", depNodeFn.name().asChar());
    }
    else
    {
        RENDERER_LOG_WARNING(
            "Found unsupported shading node %s while exporting network",
            depNodeFn.typeName().asChar());
    }
}
