
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
#include "appleseedmaya/exporters/exporterfactory.h"
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/logger.h"

namespace asf = foundation;
namespace asr = renderer;

void ShadingNetworkExporter::registerExporter()
{
    MStringArray nodeNames;
    ShadingNodeRegistry::getShaderNodeNames(nodeNames);

    for(int i = 0, e = nodeNames.length(); i < e; ++i)
    {
        NodeExporterFactory::registerMPxNodeExporter(
            nodeNames[i],
            &ShadingNetworkExporter::create);
    }
}

MPxNodeExporter *ShadingNetworkExporter::create(
    const MObject&                  object,
    asr::Project&                   project,
    AppleseedSession::SessionMode   sessionMode)
{
    return new ShadingNetworkExporter(object, project, sessionMode);
}

ShadingNetworkExporter::ShadingNetworkExporter(
    const MObject&                  object,
    asr::Project&                   project,
    AppleseedSession::SessionMode   sessionMode)
  : MPxNodeExporter(object, project, sessionMode)
{
}

void ShadingNetworkExporter::createEntity(const AppleseedSession::Options& options)
{
    MString name = appleseedName() + MString("_shader_group");
    m_shaderGroup = asr::ShaderGroupFactory::create(name.asChar());

    m_shadersExported.clear();
    createShader(node());

    m_shadersExported.clear();
    addConnections(node());
}

void ShadingNetworkExporter::flushEntity()
{
    mainAssembly().shader_groups().insert(m_shaderGroup.release());
}

void ShadingNetworkExporter::createShader(const MObject& node)
{
    MStatus status;
    MFnDependencyNode depNodeFn(node);

    const OSLShaderInfo *shaderInfo =
        ShadingNodeRegistry::getShaderInfo(depNodeFn.typeName());

    if(!shaderInfo)
    {
        std::cout << "Skipping unsupported shader: " << depNodeFn.typeName() << "\n";
        return;
    }

    if(m_shadersExported.count(depNodeFn.name()) != 0)
    {
        std::cout << "Skipping already exported shader: " << depNodeFn.name() << "\n";
        return;
    }

    m_shadersExported.insert(depNodeFn.name());

    asr::ParamArray shaderParams;

    for(int i = 0, e = shaderInfo->paramInfo.size(); i < e; ++i)
    {
        const OSLParamInfo& paramInfo = shaderInfo->paramInfo[i];

        // Skip output attributes.
        if(paramInfo.isOutput)
        {
            std::cout << "Skipping output attribute: " << "\n";
            std::cout << paramInfo << std::endl;
            continue;
        }

        if(!paramInfo.validDefault)
        {
            std::cout << "Skipping attribute without valid default: " << "\n";
            std::cout << paramInfo << std::endl;
            continue;
        }

        if(paramInfo.isArray)
        {
            std::cout << "Skipping array attribute: " << "\n";
            std::cout << paramInfo << std::endl;
            continue;
        }

        MPlug plug = depNodeFn.findPlug(paramInfo.mayaAttributeName, &status);
        if(!status)
        {
            std::cout << "Skipping unknown attribute: "
                        << paramInfo.mayaAttributeName << std::endl;
            continue;
        }

        if(plug.isConnected())
        {
            MObject srcNode;
            if(AttributeUtils::get(plug, srcNode))
                createShader(srcNode);

            continue;
        }

        if(plug.isCompound() && plug.numConnectedChildren() != 0)
        {
            std::cout << "Skipping connected compound attribute: " << plug.name() << "\n";
            continue;
        }

        if(plug.isArray() && plug.numConnectedElements() != 0)
        {
            std::cout << "Skipping connected array attribute: " << plug.name() << "\n";
            continue;
        }

        processAttribute(plug, paramInfo, shaderParams);
    }

    m_shaderGroup->add_shader(
        shaderInfo->shaderType.asChar(),
        shaderInfo->shaderName.asChar(),
        depNodeFn.name().asChar(),
        shaderParams);
}

void ShadingNetworkExporter::processAttribute(
    const MPlug&        plug,
    const OSLParamInfo& paramInfo,
    asr::ParamArray&    shaderParams)
{
    std::cout << "Processing shading node attr:" << std::endl;
    std::cout << paramInfo << std::endl;

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
        std::cout << "Skipping param of type: " << paramInfo.paramType << std::endl;

    std::string valueAsString = ss.str();
    if(!valueAsString.empty())
        shaderParams.insert(paramInfo.paramName.asChar(), ss.str().c_str());
}

void ShadingNetworkExporter::addConnections(const MObject& node)
{
    MStatus status;
    MFnDependencyNode depNodeFn(node);

    const OSLShaderInfo *shaderInfo =
        ShadingNodeRegistry::getShaderInfo(depNodeFn.typeName());

    if(!shaderInfo)
    {
        std::cout << "Skipping unsupported shader: " << depNodeFn.typeName() << "\n";
        return;
    }

    if(m_shadersExported.count(depNodeFn.name()) != 0)
    {
        std::cout << "Skipping already exported shader: " << depNodeFn.name() << "\n";
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
            std::cout << "Skipping unknown attribute: "
                        << paramInfo.mayaAttributeName << std::endl;
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
                    continue;

                const OSLParamInfo *srcParamInfo =
                    srcShaderInfo->findParam(
                        srcPlug.partialName(
                            false,
                            false,
                            false,
                            false,
                            false,
                            true,   // use long names.
                            &status));

                if(srcParamInfo)
                {
                    m_shaderGroup->add_connection(
                        srcDepNodeFn.name().asChar(),
                        srcParamInfo->paramName.asChar(),
                        depNodeFn.name().asChar(),
                        paramInfo.paramName.asChar());
                }

                addConnections(srcPlug.node());
            }
        }

        if(plug.isCompound() && plug.numConnectedChildren() != 0)
        {
            // ???
        }

        if(plug.isArray() && plug.numConnectedElements() != 0)
        {
            // ???
        }
    }
}
