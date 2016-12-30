
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

// appleseed.foundation headers.
#include "foundation/utility/string.h"

// appleseed.maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exporters/exporterfactory.h"
#include "appleseedmaya/logger.h"
#include "appleseedmaya/shadingnoderegistry.h"

namespace asf = foundation;
namespace asr = renderer;

namespace
{

const char *g_compToVectorParamNames[] = {"compX", "compY", "compZ"};
const char *g_compToVectorOutputParamName = "comp";

const char *g_vectorToCompParamNames[] = {"compX", "compY", "compZ"};
const char *g_vectorToCompInputParamName = "comp";

const char *g_compToUVParamNames[] = {"compU", "compV"};
const char *g_compToUVOutputParamName = "comp";

const char *g_uvToCompParamNames[] = {"compU", "compV"};
const char *g_uvToCompInputParamName = "comp";

}

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
    const MObject&                      object,
    asr::ShaderGroup&                   shaderGroup)
{
    return new ShadingNodeExporter(object, shaderGroup);
}

ShadingNodeExporter::ShadingNodeExporter(
    const MObject&                      object,
    asr::ShaderGroup&                   shaderGroup)
  : m_object(object)
  , m_shaderGroup(shaderGroup)
{
}

void ShadingNodeExporter::createEntities(ShadingNodeExporterMap& exporters)
{
    MStatus status;
    MFnDependencyNode depNodeFn(m_object);
    const OSLShaderInfo& shaderInfo = getShaderInfo();

    // The fact that we need to specify shaders in depth first
    // order and that Maya allows component connections but OSL
    // does not make things a bit messy...

    // - We need to create any input adaptor shader first.
    // - Then we need to create the shader itself.
    // - Output adaptor shaders are created as needed.

    // Create adaptor shaders and add component connections first.
    for(size_t i = 0, e = shaderInfo.paramInfo.size(); i < e; ++i)
    {
        const OSLParamInfo& paramInfo = shaderInfo.paramInfo[i];

        // Skip output attributes.
        if (paramInfo.isOutput)
            continue;

        MPlug plug = depNodeFn.findPlug(paramInfo.mayaAttributeName, &status);
        if (!status)
            continue;

        if (plug.isConnected())
            continue;

        if (plug.isCompound() && plug.numConnectedChildren() != 0)
        {
            if (paramInfo.paramType == "color"  ||
                paramInfo.paramType == "normal" ||
                paramInfo.paramType == "point"  ||
                paramInfo.paramType == "vector")
            {
                createInputFloatCompoundAdaptorShader(
                    paramInfo,
                    plug,
                    exporters,
                    "as_maya_components2Vector",
                    "__comp2Vector#",
                    g_compToVectorParamNames,
                    g_compToVectorOutputParamName);
            }
            else if (paramInfo.paramType == "float[2]")
            {
                createInputFloatCompoundAdaptorShader(
                    paramInfo,
                    plug,
                    exporters,
                    "as_maya_components2UV",
                    "__comp2UV#",
                    g_compToUVParamNames,
                    g_compToUVOutputParamName);
            }
            else
            {
                RENDERER_LOG_WARNING(
                    "Skipping child compound connection to attribute %s of shading node %s",
                    paramInfo.mayaAttributeName.asChar(),
                    depNodeFn.typeName().asChar());
            }
        }
        else if (plug.isArray() && plug.numConnectedElements() != 0)
        {
            // todo: implement this...
            RENDERER_LOG_WARNING(
                "Skipping array element connection to attribute %s of shading node %s",
                paramInfo.mayaAttributeName.asChar(),
                depNodeFn.typeName().asChar());
        }
    }

    // Convert the param values and create the shader for this node.
    asr::ParamArray shaderParams;
    exportShaderParameters(shaderInfo, shaderParams);

    m_shaderGroup.add_shader(
        shaderInfo.shaderType.asChar(),
        shaderInfo.shaderName.asChar(),
        depNodeFn.name().asChar(),
        shaderParams);

    // Create connections.
    for(size_t i = 0, e = shaderInfo.paramInfo.size(); i < e; ++i)
    {
        const OSLParamInfo& paramInfo = shaderInfo.paramInfo[i];

        // Skip output attributes.
        if (paramInfo.isOutput)
            continue;

        MPlug plug = depNodeFn.findPlug(paramInfo.mayaAttributeName, &status);
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
            ShadingNodeExporter *srcNodeExporter = getSrcPlugAndExporter(plug, exporters, srcPlug);
            if (!srcNodeExporter)
            {
                MFnDependencyNode srcDepNodeFn(srcPlug.node());
                RENDERER_LOG_WARNING(
                    "Skipping connections to unsupported shading node %s",
                    srcDepNodeFn.typeName().asChar());
                continue;
            }

            MString srcLayerName;
            MString srcParam;
            if (srcNodeExporter->layerAndParamNameFromPlug(srcPlug, srcLayerName, srcParam))
            {
                m_shaderGroup.add_connection(
                    srcLayerName.asChar(),
                    srcParam.asChar(),
                    depNodeFn.name().asChar(),
                    paramInfo.paramName.asChar());
            }
        }
    }
}

bool ShadingNodeExporter::layerAndParamNameFromPlug(
    const MPlug&                        plug,
    MString&                            layerName,
    MString&                            paramName)
{
    MFnDependencyNode depNodeFn(node());

    if (plug.isChild())
    {
        MPlug parentPlug = plug.parent();

        const OSLParamInfo *paramInfo = getShaderInfo().findParam(parentPlug);

        if (!paramInfo)
            return false;

        if (paramInfo->paramType == "color"  ||
            paramInfo->paramType == "normal" ||
            paramInfo->paramType == "point"  ||
            paramInfo->paramType == "vector")
        {
            layerName = "__vector2Comps#";
            return createOutputFloatCompoundAdaptorShader(
                plug,
                "as_maya_vector2Components",
                g_vectorToCompParamNames,
                g_vectorToCompInputParamName,
                layerName,
                paramName);
        }
        else if (paramInfo->paramType == "float[2]")
        {
            layerName = "__uv2Comps#";
            return createOutputFloatCompoundAdaptorShader(
                plug,
                "as_maya_uv2Components",
                g_uvToCompParamNames,
                g_uvToCompInputParamName,
                layerName,
                paramName);
        }
        else
        {
            RENDERER_LOG_WARNING(
                "Skipping compound child connection to attribute %s of shading node %s",
                plug.name().asChar(),
                depNodeFn.typeName().asChar());
            return false;
        }
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

void ShadingNodeExporter::exportShaderParameters(
    const OSLShaderInfo&                shaderInfo,
    asr::ParamArray&                    shaderParams) const
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
    const MPlug&                        plug,
    const OSLParamInfo&                 paramInfo,
    renderer::ParamArray&               shaderParams) const
{
    // Skip params with shader global defaults.
    if (!paramInfo.validDefault)
        return;

    // Skip connected attributes.
    if (plug.isConnected())
        return;

    // Skip output attributes.
    if (paramInfo.isOutput)
        return;

    if (paramInfo.isArray)
        exportArrayValue(plug, paramInfo, shaderParams);
    else
        exportValue(plug, paramInfo, shaderParams);
}

void ShadingNodeExporter::exportValue(
    const MPlug&                        plug,
    const OSLParamInfo&                 paramInfo,
    asr::ParamArray&                    shaderParams) const
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
    const MPlug&                        plug,
    const OSLParamInfo&                 paramInfo,
    asr::ParamArray&                    shaderParams) const
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

MObject ShadingNodeExporter::node() const
{
    return m_object;
}

const OSLShaderInfo& ShadingNodeExporter::getShaderInfo() const
{
    MFnDependencyNode depNodeFn(m_object);
    const OSLShaderInfo *shaderInfo = ShadingNodeRegistry::getShaderInfo(depNodeFn.typeName());
    assert(shaderInfo);

    return *shaderInfo;
}

ShadingNodeExporter *ShadingNodeExporter::findExporterForNode(
    ShadingNodeExporterMap&             exporters,
    const MObject&                      node)
{
    MFnDependencyNode depNodeFn(node);
    ShadingNodeExporterMap::const_iterator it;
    it = exporters.find(depNodeFn.name());

    if (it != exporters.end())
        return it->second;

    return 0;
}

ShadingNodeExporter *ShadingNodeExporter::getSrcPlugAndExporter(
    const MPlug&                        plug,
    ShadingNodeExporterMap&             exporters,
    MPlug&                              srcPlug)
{
    MStatus status;
    MPlugArray inputConnections;
    plug.connectedTo(inputConnections, true, false, &status);
    srcPlug = inputConnections[0];
    return findExporterForNode(exporters, srcPlug.node());
}

MString ShadingNodeExporter::createAdaptorShader(
    const MString&                      shaderName,
    const MString&                      layerName,
    const renderer::ParamArray&         params)
{
    std::string uniqueLayerName = asf::get_numbered_string(
        layerName.asChar(),
        m_shaderGroup.shaders().size());

    m_shaderGroup.add_shader(
        "shader",
        shaderName.asChar(),
        uniqueLayerName.c_str(),
        params);

    return uniqueLayerName.c_str();
}

void ShadingNodeExporter::createInputFloatCompoundAdaptorShader(
    const OSLParamInfo&                 paramInfo,
    const MPlug&                        plug,
    ShadingNodeExporterMap&             exporters,
    const MString&                      shaderName,
    const MString&                      layerName,
    const char**                        shaderParamNames,
    const char*                         shaderOutputParamName)
{
    MFnDependencyNode depNodeFn(node());

    asr::ParamArray params;
    std::vector<MString> srcLayerNames;
    std::vector<MString> srcParamNames;
    std::vector<size_t>  dstParamIndices;

    // For each child attribute.
    for (size_t i = 0, e = plug.numChildren(); i < e; ++i)
    {
        MPlug childPlug = plug.child(i);
        if (childPlug.isConnected())
        {
            MPlug srcPlug;
            ShadingNodeExporter *srcNodeExporter = getSrcPlugAndExporter(childPlug, exporters, srcPlug);
            if (!srcNodeExporter)
            {
                MFnDependencyNode srcDepNodeFn(srcPlug.node());
                RENDERER_LOG_WARNING(
                    "Skipping connections to unsupported shading node %s",
                    srcDepNodeFn.typeName().asChar());
                continue;
            }

            // Find the layer and param names on the other side of the connection.
            MString srcLayerName;
            MString srcParam;
            if (srcNodeExporter->layerAndParamNameFromPlug(srcPlug, srcLayerName, srcParam))
            {
                // We cannot do make a connection now because we don't know
                // the name of the destination layer (has not been created yet).
                // Save the info we need to make a connection later.
                srcLayerNames.push_back(srcLayerName);
                srcParamNames.push_back(srcParam);
                dstParamIndices.push_back(i);
            }
        }
        else
        {
            // Save the value of this child attribute as a shader param.
            float value;
            if (AttributeUtils::get(childPlug, value))
            {
                std::stringstream ss;
                ss << "float " << value;
                params.insert(shaderParamNames[i], ss.str().c_str());
            }
        }
    }

    const MString adaptorName = createAdaptorShader(
        shaderName,
        layerName,
        params);

    for (size_t j = 0; j < dstParamIndices.size(); ++j)
    {
        m_shaderGroup.add_connection(
            srcLayerNames[j].asChar(),
            srcParamNames[j].asChar(),
            adaptorName.asChar(),
            shaderParamNames[dstParamIndices[j]]);
    }

    m_shaderGroup.add_connection(
        adaptorName.asChar(),
        shaderOutputParamName,
        depNodeFn.name().asChar(),
        paramInfo.paramName.asChar());
}

bool ShadingNodeExporter::createOutputFloatCompoundAdaptorShader(
    const MPlug&                        plug,
    const MString&                      shaderName,
    const char**                        shaderParamNames,
    const char*                         shaderInputParamName,
    MString&                            layerName,
    MString&                            paramName)
{
    MPlug parentPlug = plug.parent();

    MString srcLayerName;
    MString srcParamName;
    if (layerAndParamNameFromPlug(parentPlug, srcLayerName, srcParamName) == false)
        return false;

    for (size_t i = 0, e = parentPlug.numChildren(); i < e; ++i)
    {
        if (plug == parentPlug.child(i))
        {
            paramName = shaderParamNames[i];
            break;
        }
    }

    if (paramName.length() == 0)
        return false;

    layerName = createAdaptorShader(
        shaderName,
        layerName,
        asr::ParamArray());
    m_shaderGroup.add_connection(
        srcLayerName.asChar(),
        srcParamName.asChar(),
        layerName.asChar(),
        shaderInputParamName);

    return true;
}
