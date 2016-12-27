
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
#include "appleseedmaya/exporters/shadingnodeexporter.h"

// Standard headers.
#include <sstream>

// Maya headers.
#include <maya/MFnDependencyNode.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnNumericAttribute.h>

// appleseed.maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exporters/exporterfactory.h"
#include "appleseedmaya/logger.h"
#include "appleseedmaya/shadingnoderegistry.h"

namespace asf = foundation;
namespace asr = renderer;

void ShadingNodeExporter::registerExporters()
{
    MStringArray nodeNames;
    ShadingNodeRegistry::getShaderNodeNames(nodeNames);

    for(int i = 0, e = nodeNames.length(); i < e; ++i)
    {
        NodeExporterFactory::registerShadingNodeExporter(
            nodeNames[i],
            &ShadingNodeExporter::create);
    }
}

ShadingNodeExporter *ShadingNodeExporter::create(
    const MObject&          object,
    asr::ShaderGroup&       shaderGroup)
{
    return new ShadingNodeExporter(object, shaderGroup);
}

ShadingNodeExporter::ShadingNodeExporter(
    const MObject&          object,
    asr::ShaderGroup&       shaderGroup)
  : m_object(object)
  , m_shaderGroup(shaderGroup)
{
}

MObject ShadingNodeExporter::node() const
{
    return m_object;
}

const OSLShaderInfo&ShadingNodeExporter::getShaderInfo() const
{
    MFnDependencyNode depNodeFn(m_object);
    const OSLShaderInfo *shaderInfo = ShadingNodeRegistry::getShaderInfo(depNodeFn.typeName());
    assert(shaderInfo);

    return *shaderInfo;
}

void ShadingNodeExporter::createShader()
{
    MStatus status;
    MFnDependencyNode depNodeFn(m_object);

    const OSLShaderInfo& shaderInfo = getShaderInfo();

    // Create input adaptor shaders.
    for (int i = 0, e = shaderInfo.paramInfo.size(); i < e; ++i)
    {
        const OSLParamInfo& paramInfo = shaderInfo.paramInfo[i];

        if (paramInfo.isOutput)
            continue;

        MPlug plug = depNodeFn.findPlug(paramInfo.mayaAttributeName, &status);
        if (!status)
            continue;

        if (plug.isCompound() && plug.numConnectedChildren() != 0)
        {
            // createAdaptorShader();
        }
        else if (plug.isArray() && plug.numConnectedElements() != 0)
        {
            // createAdaptorShader();
        }
    }

    asr::ParamArray shaderParams;
    exportShaderParameters(shaderInfo, shaderParams);

    m_shaderGroup.add_shader(
        shaderInfo.shaderType.asChar(),
        shaderInfo.shaderName.asChar(),
        depNodeFn.name().asChar(),
        shaderParams);

    // Create output adaptor shaders here.
    for (int i = 0, e = shaderInfo.paramInfo.size(); i < e; ++i)
    {
        const OSLParamInfo& paramInfo = shaderInfo.paramInfo[i];

        if (!paramInfo.isOutput)
            continue;

        MPlug plug = depNodeFn.findPlug(paramInfo.mayaAttributeName, &status);
        if (!status)
            continue;

        if (plug.isCompound() && plug.numConnectedChildren() != 0)
        {
            // createAdaptorShader();
        }
        else if (plug.isArray() && plug.numConnectedElements() != 0)
        {
            // createAdaptorShader();
        }
    }
}

void ShadingNodeExporter::exportShaderParameters(
    const OSLShaderInfo&    shaderInfo,
    asr::ParamArray&        shaderParams) const
{
    MStatus status;
    MFnDependencyNode depNodeFn(m_object);

    for(int i = 0, e = shaderInfo.paramInfo.size(); i < e; ++i)
    {
        const OSLParamInfo& paramInfo = shaderInfo.paramInfo[i];
        MPlug plug = depNodeFn.findPlug(paramInfo.mayaAttributeName, &status);
        if (!status)
        {
            RENDERER_LOG_WARNING(
                "Skipping unknown attribute %s of shading node %s",
                paramInfo.mayaAttributeName.asChar(),
                depNodeFn.typeName().asChar());
            continue;
        }

        exportParameterValue(plug, paramInfo, shaderParams);
    }
}

void ShadingNodeExporter::exportParameterValue(
    const MPlug&            plug,
    const OSLParamInfo&     paramInfo,
    renderer::ParamArray&   shaderParams) const
{
    // Skip params with shader global defaults.
    if (!paramInfo.validDefault)
        return;

    // Skip connected attributes.
    if (plug.isConnected())
        return;

    // Create adapter shaders.
    if (plug.isCompound() && plug.numConnectedChildren() != 0)
    {
        // todo: implement...
    }

    // Skip output attributes.
    if (paramInfo.isOutput)
        return;

    if (paramInfo.isArray)
        exportArrayValue(plug, paramInfo, shaderParams);
    else
        exportValue(plug, paramInfo, shaderParams);
}

void ShadingNodeExporter::exportValue(
    const MPlug&            plug,
    const OSLParamInfo&     paramInfo,
    asr::ParamArray&        shaderParams) const
{
    RENDERER_LOG_DEBUG(
        "Exporting shading node attr %s.",
        paramInfo.mayaAttributeName.asChar());

    std::stringstream ss;

    if (paramInfo.paramType == "color")
    {
        MColor value;
        if (AttributeUtils::get(plug, value))
            ss << "color " << value.r << " " << value.g << " " << value.b;
    }
    else if (paramInfo.paramType == "float")
    {
        if (paramInfo.mayaAttributeType == "angle")
        {
            MAngle value(0.0f, MAngle::kDegrees);
            if (AttributeUtils::get(plug, value))
                ss << "float " << value.asDegrees();
        }
        else
        {
            float value;
            if (AttributeUtils::get(plug, value))
                ss << "float " << value;
        }
    }
    else if (paramInfo.paramType == "int")
    {
        int value;
        if (AttributeUtils::get(plug, value))
            ss << "int " << value;
        else
        {
            bool boolValue;
            if (AttributeUtils::get(plug, boolValue))
                ss << "int " << boolValue ? "1" : "0";
        }
    }
    else if (paramInfo.paramType == "matrix")
    {
        MMatrix matrixValue;
        if (AttributeUtils::get(plug, matrixValue))
        {
            ss << "matrix ";
            for(int i = 0; i < 4; ++i)
                for(int j = 0; j < 4; ++j)
                    ss << matrixValue[i][j] << " ";
        }
    }
    else if (paramInfo.paramType == "normal")
    {
        MVector value;
        if (AttributeUtils::get(plug, value))
            ss << "normal " << value.z << " " << value.y << " " << value.z;
    }
    else if (paramInfo.paramType == "point")
    {
        MPoint value;
        if (AttributeUtils::get(plug, value))
            ss << "point " << value.z << " " << value.y << " " << value.z;
    }
    else if (paramInfo.paramType == "string")
    {
        if (paramInfo.widget == "popup")
        {
            MObject attr = plug.attribute();
            MFnEnumAttribute fnEnumAttr(attr);
            int intValue = plug.asInt();
            MString value = fnEnumAttr.fieldName(intValue);
            ss << "string " << value;
        }
        else
        {
            MString value;
            if (AttributeUtils::get(plug, value))
                ss << "string " << value;
        }
    }
    else if (paramInfo.paramType == "vector")
    {
        MVector value;
        if (AttributeUtils::get(plug, value))
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
    if (!valueAsString.empty())
        shaderParams.insert(paramInfo.paramName.asChar(), valueAsString.c_str());
}

void ShadingNodeExporter::exportArrayValue(
    const MPlug&            plug,
    const OSLParamInfo&     paramInfo,
    asr::ParamArray&        shaderParams) const
{
    RENDERER_LOG_DEBUG(
        "Exporting shading node attr %s.",
        paramInfo.mayaAttributeName.asChar());

    MStatus status;
    bool valid = true;

    std::stringstream ss;

    if (strncmp(paramInfo.paramType.asChar(), "float[", 5) == 0)
    {
        assert(plug.isCompound());

        ss << "float[] ";
        for(size_t i = 0, e = plug.numChildren(); i < e; ++i)
        {
            MPlug childPlug = plug.child(i, &status);
            if (status)
            {
                float value;
                if (AttributeUtils::get(childPlug, value))
                    ss << value << " ";
                else
                    valid = false;
            }
            else
                valid = false;
        }
    }
    else if (strncmp(paramInfo.paramType.asChar(), "int[", 4) == 0)
    {
        assert(plug.isCompound());

        ss << "int[] ";
        for(size_t i = 0, e = plug.numChildren(); i < e; ++i)
        {
            MPlug childPlug = plug.child(i, &status);
            if (status)
            {
                int value;
                if (AttributeUtils::get(childPlug, value))
                    ss << value << " ";
                else
                    valid = false;
            }
            else
                valid = false;
        }
    }
    else
    {
        RENDERER_LOG_WARNING(
            "Skipping shading node attr %s of type %s.",
            paramInfo.mayaAttributeName.asChar(),
            paramInfo.paramType.asChar());
        return;
    }

    if (valid)
    {
        std::string valueAsString = ss.str();
        if (!valueAsString.empty())
            shaderParams.insert(paramInfo.paramName.asChar(), valueAsString.c_str());
    }
    else
    {
        RENDERER_LOG_WARNING(
            "Error exporting shading node attr %s of type %s.",
            paramInfo.mayaAttributeName.asChar(),
            paramInfo.paramType.asChar());
    }
}

bool ShadingNodeExporter::layerAndParamNameFromPlug(
    const MPlug&             plug,
    MString&                 layerName,
    MString&                 paramName) const
{
    MFnDependencyNode depNodeFn(node());

    if (plug.isChild())
    {
        RENDERER_LOG_WARNING(
            "Skipping compound child connection to attribute %s of shading node %s",
            plug.name().asChar(),
            depNodeFn.typeName().asChar());
        return false;
    }

    if (plug.isElement())
    {
        RENDERER_LOG_WARNING(
            "Skipping array element connection to attribute %s of shading node %s",
            plug.name().asChar(),
            depNodeFn.typeName().asChar());
        return false;
    }

    if (const OSLParamInfo *paramInfo = getShaderInfo().findParam(plug))
    {
        layerName = depNodeFn.name();
        paramName = paramInfo->paramName;
        return true;
    }

    return false;
}

void ShadingNodeExporter::exportInputAdaptorConnections()
{
    // todo: implement...
}

void ShadingNodeExporter::exportOutputAdaptorConnections()
{
    // todo: implement...
}
