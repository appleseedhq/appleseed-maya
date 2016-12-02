
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
#include <iostream>
#include <sstream>

// Maya headers.
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>

// appleseed.renderer headers.
#include "renderer/api/material.h"
#include "renderer/api/scene.h"

// appleseed.maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/logger.h"

namespace asf = foundation;
namespace asr = renderer;

ShadingNetworkExporter::ShadingNetworkExporter(
    const Context                   context,
    const MObject&                  object,
    asr::Project&                   project,
    AppleseedSession::SessionMode   sessionMode)
  : MPxNodeExporter(object, project, sessionMode)
  , m_context(context)
{
}

MString ShadingNetworkExporter::shaderGroupName() const
{
    return m_shaderGroup->get_name();
}

void ShadingNetworkExporter::createEntity(const AppleseedSession::Options& options)
{
    MString name = appleseedName() + MString("_shader_group");
    m_shaderGroup = asr::ShaderGroupFactory::create(name.asChar());

    m_shadersExported.clear();
    exportShader(node());

    m_shadersExported.clear();
    m_numAdaptersCreated = 1;
    exportConnections(node());
}

void ShadingNetworkExporter::flushEntity()
{
    insertEntityWithUniqueName(
        mainAssembly().shader_groups(),
        m_shaderGroup);
}

void ShadingNetworkExporter::exportShader(const MObject& node)
{
    MStatus status;
    MFnDependencyNode depNodeFn(node);

    const OSLShaderInfo *shaderInfo =
        ShadingNodeRegistry::getShaderInfo(depNodeFn.typeName());

    if(!shaderInfo)
    {
        RENDERER_LOG_WARNING(
            "Unsupported shading node type %s found",
            depNodeFn.typeName().asChar());
        return;
    }

    if(m_shadersExported.count(depNodeFn.name()) != 0)
    {
        RENDERER_LOG_DEBUG(
            "Skipping already exported shading node %s.",
            depNodeFn.name().asChar());
        return;
    }

    m_shadersExported.insert(depNodeFn.name());

    asr::ParamArray shaderParams;
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

        // Export shading nodes connected to this attribute.
        if(plug.isConnected())
        {
            MObject srcNode;
            if(AttributeUtils::get(plug, srcNode))
                exportShader(srcNode);

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
                        exportShader(srcNode);
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
                        exportShader(srcNode);
                }
            }
        }

        if(plug.isArray())
            exportArrayAttributeValue(plug, paramInfo, shaderParams);
        else exportAttributeValue(plug, paramInfo, shaderParams);
    }

    m_shaderGroup->add_shader(
        shaderInfo->shaderType.asChar(),
        shaderInfo->shaderName.asChar(),
        depNodeFn.name().asChar(),
        shaderParams);
}

void ShadingNetworkExporter::exportAttributeValue(
    const MPlug&        plug,
    const OSLParamInfo& paramInfo,
    asr::ParamArray&    shaderParams)
{
    RENDERER_LOG_DEBUG(
        "Exporting shading node attr %s.",
        paramInfo.mayaAttributeName.asChar());

    std::stringstream ss;

    if(strcmp(paramInfo.paramType.asChar(), "color") == 0)
    {
        MColor value;
        if(AttributeUtils::get(plug, value))
            ss << "color " << value.r << " " << value.g << " " << value.b;
    }
    else if(strcmp(paramInfo.paramType.asChar(), "float") == 0)
    {
        float value;
        if(AttributeUtils::get(plug, value))
            ss << "float " << value;
    }
    else if(strcmp(paramInfo.paramType.asChar(), "int") == 0)
    {
        int value;
        if(AttributeUtils::get(plug, value))
            ss << "int " << value;
    }
    else if(strcmp(paramInfo.paramType.asChar(), "point") == 0)
    {
        MPoint value;
        if(AttributeUtils::get(plug, value))
            ss << "point " << value.z << " " << value.y << " " << value.z;
    }
    else if(strcmp(paramInfo.paramType.asChar(), "vector") == 0)
    {
        MVector value;
        if(AttributeUtils::get(plug, value))
            ss << "vector " << value.z << " " << value.y << " " << value.z;
    }
    else
    {
        RENDERER_LOG_WARNING(
            "Skipping shading node attr %s of unknown type %s.",
            paramInfo.mayaAttributeName.asChar(),
            paramInfo.paramType.asChar());
    }

    std::string valueAsString = ss.str();
    if(!valueAsString.empty())
        shaderParams.insert(paramInfo.paramName.asChar(), ss.str().c_str());
}

void ShadingNetworkExporter::exportArrayAttributeValue(
    const MPlug&        plug,
    const OSLParamInfo& paramInfo,
    asr::ParamArray&    shaderParams)
{
    RENDERER_LOG_DEBUG(
        "Exporting shading node array attr %s.",
        paramInfo.mayaAttributeName.asChar());

    RENDERER_LOG_WARNING(
        "Skipping shading node array attr %s of unknown type %s.",
        paramInfo.mayaAttributeName.asChar(),
        paramInfo.paramType.asChar());
}

void ShadingNetworkExporter::exportConnections(const MObject& node)
{
    MStatus status;
    MFnDependencyNode depNodeFn(node);

    const OSLShaderInfo *shaderInfo =
        ShadingNodeRegistry::getShaderInfo(depNodeFn.typeName());

    if(!shaderInfo)
    {
        RENDERER_LOG_WARNING(
            "Unsupported shading node type %s found",
            depNodeFn.typeName().asChar());
        return;
    }

    if(m_shadersExported.count(depNodeFn.name()) != 0)
    {
        RENDERER_LOG_DEBUG(
            "Skipping connections for already exported shading node %s.",
            depNodeFn.name().asChar());
        return;
    }

    m_shadersExported.insert(depNodeFn.name());

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
            MPlug srcPlug;
            if(AttributeUtils::getPlugConnectedTo(plug, srcPlug))
            {
                MFnDependencyNode srcDepNodeFn(srcPlug.node());

                const OSLShaderInfo *srcShaderInfo =
                    ShadingNodeRegistry::getShaderInfo(srcDepNodeFn.typeName());

                if(!srcShaderInfo)
                {
                    RENDERER_LOG_WARNING(
                        "Unsupported shading node type %s found",
                        srcDepNodeFn.typeName().asChar());
                    continue;
                }

                const MString srcPlugName =
                    srcPlug.partialName(
                        false,
                        false,
                        false,
                        false,
                        false,
                        true,   // use long names.
                        &status);

                if(const OSLParamInfo *srcParamInfo = srcShaderInfo->findParam(srcPlugName))
                {
                    m_shaderGroup->add_connection(
                        srcDepNodeFn.name().asChar(),
                        srcParamInfo->paramName.asChar(),
                        depNodeFn.name().asChar(),
                        paramInfo.paramName.asChar());

                    exportConnections(srcPlug.node());
                }
                else
                {
                    RENDERER_LOG_DEBUG(
                        "Skipping unknown attribute %s of shading node %s",
                        srcPlugName.asChar(),
                        srcDepNodeFn.typeName().asChar());
                }
            }
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
                        exportConnections(srcNode);
                }
            }

            // Create adapter node here...
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
                        exportConnections(srcNode);
                }
            }
        }
    }
}

void ShadingNetworkExporter::createCompoundChildConnectionAdapterShader(
    const MPlug&          plug,
    const OSLParamInfo&   paramInfo)
{
    MFnDependencyNode depNodeFn(plug.node());

    //TODO:
    //    - Create adapter with defaults == plug value.
    m_numAdaptersCreated++;
    //    - Add connection between node and adapter.
    //    - Connect src shader connections
}
