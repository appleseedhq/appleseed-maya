
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

// Maya headers.
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>

// appleseed.renderer headers.
#include "renderer/api/material.h"
#include "renderer/api/scene.h"

// appleseed.maya headers.
#include "appleseedmaya/exporters/exporterfactory.h"

#include <iostream>

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

void ShadingNetworkExporter::createEntity()
{
    MString name = appleseedName() + MString("_shader_group");
    m_shaderGroup = asr::ShaderGroupFactory::create(name.asChar());
    createShader(node());
}

void ShadingNetworkExporter::flushEntity()
{
    mainAssembly().shader_groups().insert(m_shaderGroup.release());
}

void ShadingNetworkExporter::createShader(const MObject& object)
{
    MFnDependencyNode depNodeFn(object);

    const OSLShaderInfo *shaderInfo =
        ShadingNodeRegistry::getShaderInfo(depNodeFn.typeName());

    if(shaderInfo)
    {
        asr::ParamArray shaderParams;

        for(int i = 0, e = shaderInfo->paramInfo.size(); i < e; ++i)
        {
            const OSLParamInfo& paramInfo = shaderInfo->paramInfo[i];

            // Skip output attributes.
            if(paramInfo.isOutput)
                continue;

            if(paramInfo.isArray)
                processArrayAttribute(object, paramInfo);
            else
                processAttribute(object, paramInfo);
        }

        m_shaderGroup->add_shader(
            shaderInfo->shaderType.asChar(),
            shaderInfo->shaderName.asChar(),
            depNodeFn.name().asChar(),
            shaderParams);
    }
    else
    {
        // warning here...!
    }
}

void ShadingNetworkExporter::processAttribute(const MObject& object, const OSLParamInfo& paramInfo)
{
    const MString& attrName = paramInfo.mayaAttributeName;
    std::cout << "Processing shading node attr: " << attrName << ", type = " << paramInfo.paramType << std::endl;
    // ...
}

void ShadingNetworkExporter::processArrayAttribute(const MObject& object, const OSLParamInfo& paramInfo)
{
    const MString& attrName = paramInfo.mayaAttributeName;
    std::cout << "Processing shading node array attr: " << attrName << ", type = " << paramInfo.paramType << std::endl;
    // ...
}
