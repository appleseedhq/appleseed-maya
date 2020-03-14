
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
#include "shadingnodeexporter.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exporters/exporterfactory.h"
#include "appleseedmaya/logger.h"
#include "appleseedmaya/ramputils.h"
#include "appleseedmaya/shadingnodemetadata.h"
#include "appleseedmaya/shadingnoderegistry.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/shadergroup.h"
#include "renderer/api/utility.h"

// appleseed.foundation headers.
#include "foundation/string/string.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MFnDependencyNode.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MRampAttribute.h>
#include <maya/MStringArray.h>
#include "appleseedmaya/_endmayaheaders.h"

// Standard headers.
#include <algorithm>
#include <sstream>
#include <vector>

namespace asf = foundation;
namespace asr = renderer;

namespace
{
    const char* g_colorComponentNames[] = {"compR", "compG", "compB"};
    const char* g_vectorComponentNames[] = {"compX", "compY", "compZ"};
    const char* g_uvComponentNames[] = {"compU", "compV"};
    const char* g_componentParamName = "comp";
}

void ShadingNodeExporter::registerExporters()
{
    MStringArray nodeNames;
    ShadingNodeRegistry::getShaderNodeNames(nodeNames);

    for (unsigned int i = 0, e = nodeNames.length(); i < e; ++i)
    {
        NodeExporterFactory::registerShadingNodeExporter(
            nodeNames[i],
            &ShadingNodeExporter::create);
    }
}

ShadingNodeExporter* ShadingNodeExporter::create(
    const MObject&                      object,
    asr::ShaderGroup&                   shaderGroup)
{
    return new ShadingNodeExporter(object, shaderGroup);
}

ShadingNodeExporter::~ShadingNodeExporter()
{
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
    // does not, make things a bit messy...

    // - We need to create any input adaptor shader first.
    // - Then we need to create the shader itself.
    // - Output adaptor shaders are created as needed.

    // Create adaptor shaders and add component connections first.
    for (size_t i = 0, e = shaderInfo.paramInfo.size(); i < e; ++i)
    {
        const OSLParamInfo& paramInfo = shaderInfo.paramInfo[i];

        // Skip output attributes.
        if (paramInfo.isOutput)
            continue;

        MPlug plug = depNodeFn.findPlug(paramInfo.mayaAttributeName, false, &status);
        if (!status)
            continue;

        if (hasConnections(plug, true, false))
        {
            // If the attribute is a float attribute,
            // check for component connections on the other side.
            if (paramInfo.paramType == "float")
            {
                MPlugArray inputConnections;
                plug.connectedTo(inputConnections, true, false, &status);

                if (!status || inputConnections.length() == 0)
                    continue;

                const MPlug otherPlug = inputConnections[0];

                if (otherPlug.isChild())
                {
                    MPlug srcPlug;
                    ShadingNodeExporter* srcNodeExporter = getSrcPlugAndExporter(plug, exporters, srcPlug);
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
                else if (otherPlug.isElement())
                {
                    // todo: implement this...
                    RENDERER_LOG_WARNING(
                        "Skipping array element connection to attribute %s of shading node %s",
                        paramInfo.mayaAttributeName.asChar(),
                        depNodeFn.typeName().asChar());
                }

                continue;
            }
            else
            {
                // Skip non float connected plugs.
                continue;
            }
        }

        if (plug.isCompound() && hasChildrenConnections(plug, true, false))
        {
            if (paramInfo.paramType == "color")
            {
                createInputFloatCompoundAdaptorShader(
                    paramInfo,
                    plug,
                    exporters,
                    "as_maya_components2Color",
                    "__comp2Color#",
                    g_colorComponentNames,
                    g_componentParamName);
            }
            else if (
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
                    g_vectorComponentNames,
                    g_componentParamName);
            }
            else if (paramInfo.paramType == "float[2]")
            {
                createInputFloatCompoundAdaptorShader(
                    paramInfo,
                    plug,
                    exporters,
                    "as_maya_components2UV",
                    "__comp2UV#",
                    g_uvComponentNames,
                    g_componentParamName);
            }
            else
            {
                RENDERER_LOG_WARNING(
                    "Skipping child compound connection to attribute %s of shading node %s",
                    paramInfo.mayaAttributeName.asChar(),
                    depNodeFn.typeName().asChar());
            }
        }
        else if (plug.isArray() && hasElementConnections(plug, true, false))
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
        shaderInfo.shaderFileName.asChar(),
        depNodeFn.name().asChar(),
        shaderParams);

    // Create connections.
    for (size_t i = 0, e = shaderInfo.paramInfo.size(); i < e; ++i)
    {
        const OSLParamInfo& paramInfo = shaderInfo.paramInfo[i];

        // Skip output attributes.
        if (paramInfo.isOutput)
            continue;

        MPlug plug = depNodeFn.findPlug(paramInfo.mayaAttributeName, false, &status);
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
            ShadingNodeExporter* srcNodeExporter = getSrcPlugAndExporter(plug, exporters, srcPlug);
            if (!srcNodeExporter)
            {
                MFnDependencyNode srcDepNodeFn(srcPlug.node());
                RENDERER_LOG_WARNING(
                    "Skipping connections to unsupported shading node %s",
                    srcDepNodeFn.typeName().asChar());
                continue;
            }

            // We already handled this case.
            if (srcPlug.isChild() || srcPlug.isElement())
                continue;

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

    // Special case for Maya's bump2d node.
    if (depNodeFn.typeName() == "bump2d")
    {
        // If we have a bumpValue connection.
        MPlug plug = depNodeFn.findPlug("bumpValue", false, &status);
        if (plug.isConnected())
        {
            // Find the node on the other side.
            MPlug srcPlug;
            ShadingNodeExporter* srcNodeExporter = getSrcPlugAndExporter(plug, exporters, srcPlug);
            if (!srcNodeExporter)
                return;

            // Check if the node has an outColor attribute.
            MFnDependencyNode otherDepNodeFn(srcPlug.node());
            srcPlug = otherDepNodeFn.findPlug("outColor", false, &status);
            if (!status)
                return;

            // Try to connect outColor to our normalMap extension attribute.
            MString srcLayerName;
            MString srcParam;
            if (srcNodeExporter->layerAndParamNameFromPlug(srcPlug, srcLayerName, srcParam))
            {
                m_shaderGroup.add_connection(
                    srcLayerName.asChar(),
                    srcParam.asChar(),
                    depNodeFn.name().asChar(),
                    "in_normalMap");
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

        const OSLParamInfo* paramInfo = getShaderInfo().findParam(parentPlug);

        if (!paramInfo)
            return false;

        if (paramInfo->paramType == "color")
        {
            layerName = "__color2Comps#";
            return createOutputFloatCompoundAdaptorShader(
                plug,
                "as_maya_color2Components",
                g_colorComponentNames,
                g_componentParamName,
                layerName,
                paramName);
        }
        else if (
            paramInfo->paramType == "normal" ||
            paramInfo->paramType == "point"  ||
            paramInfo->paramType == "vector")
        {
            layerName = "__vector2Comps#";
            return createOutputFloatCompoundAdaptorShader(
                plug,
                "as_maya_vector2Components",
                g_vectorComponentNames,
                g_componentParamName,
                layerName,
                paramName);
        }
        else if (paramInfo->paramType == "float[2]")
        {
            layerName = "__uv2Comps#";
            return createOutputFloatCompoundAdaptorShader(
                plug,
                "as_maya_uv2Components",
                g_uvComponentNames,
                g_componentParamName,
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

    if (const OSLParamInfo* paramInfo = getShaderInfo().findParam(plug))
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

    for (size_t i = 0, e = shaderInfo.paramInfo.size(); i < e; ++i)
    {
        const OSLParamInfo& paramInfo = shaderInfo.paramInfo[i];
        MPlug plug = depNodeFn.findPlug(paramInfo.mayaAttributeName, false, &status);
        if (!status)
        {
            RENDERER_LOG_WARNING(
                "Skipping unknown attribute %s of shading node %s",
                paramInfo.mayaAttributeName.asChar(),
                depNodeFn.typeName().asChar());
            continue;
        }

        // Skip params with shader globals defaults.
        if (!paramInfo.validDefault)
            continue;

        // Skip output attributes.
        if (paramInfo.isOutput)
            continue;

        exportParameterValue(plug, paramInfo, shaderParams);
    }
}

void ShadingNodeExporter::exportParameterValue(
    const MPlug&                        plug,
    const OSLParamInfo&                 paramInfo,
    renderer::ParamArray&               shaderParams) const
{
    if (paramInfo.asWidget == "ramp")
        exportRampValue(plug, paramInfo, shaderParams);
    else if (paramInfo.asWidget == "ramp_positions")
    {
        // Ramp positions are saved as part of the ramp.
        return;
    }
    else if (paramInfo.asWidget == "ramp_basis")
    {
        // Ramp basis is saved as part of the ramp.
        return;
    }
    else
    {
        if (paramInfo.isArray)
            exportArrayValue(plug, paramInfo, shaderParams);
        else
            exportValue(plug, paramInfo, shaderParams);
    }
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
        if (paramInfo.units == "degrees")
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
                ss << "int " << (boolValue ? "1" : "0"); // ?: has lower precedence than <<
        }
    }
    else if (paramInfo.paramType == "matrix")
    {
        MMatrix matrixValue;
        if (AttributeUtils::get(plug, matrixValue))
        {
            ss << "matrix ";
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
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
            short shortValue = plug.asShort();
            MString value = fnEnumAttr.fieldName(shortValue);
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
            ss << "vector " << value.x << " " << value.y << " " << value.z;
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
        for (unsigned int i = 0, e = plug.numChildren(); i < e; ++i)
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
        for (unsigned int i = 0, e = plug.numChildren(); i < e; ++i)
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

namespace
{
    template <typename T>
    void getRampValues(
        MRampAttribute&             ramp,
        std::vector<RampEntry<T>>&  entries)
    {
        entries.clear();
        entries.reserve(ramp.getNumEntries());

        MIntArray indices;
        MFloatArray positions;
        MIntArray interps;

        typename RampEntryTraits<T>::ArrayType values;

        ramp.getEntries(indices, positions, values, interps);

        for (unsigned int i = 0, e = ramp.getNumEntries(); i < e ; ++i)
            entries.push_back(RampEntry<T>(indices[i], positions[i], values[i]));

        std::sort(entries.begin(), entries.end());

        // Repeat first and last values if needed.
        // TODO: this really depends on basis...

        if (entries.size() < 4)
            entries.insert(entries.begin(), entries.front());

        if (entries.size() < 4)
            entries.push_back(entries.back());
    }
}

void ShadingNodeExporter::exportRampValue(
    const MPlug&                    plug,
    const OSLParamInfo&             paramInfo,
    renderer::ParamArray&           shaderParams) const
{
    MRampAttribute ramp(plug);
    std::string values;
    std::string positions;
    // std::string basis;

    if (paramInfo.paramType == "color[]")
    {
        std::vector<RampEntry<MColor>> entries;
        getRampValues(ramp, entries);
        serializeRamp(entries, values, positions);
    }
    else if (paramInfo.paramType == "float[]")
    {
        std::vector<RampEntry<float>> entries;
        getRampValues(ramp, entries);
        serializeRamp(entries, values, positions);
    }
    else
    {
        RENDERER_LOG_WARNING(
            "Skipping shading node attr %s of unknown type %s ramp.",
            paramInfo.mayaAttributeName.asChar(),
            paramInfo.paramType.asChar());
        return;
    }

    shaderParams.insert(paramInfo.paramName.asChar(), values.c_str());

    std::string positionsParamName = asf::replace(
        paramInfo.paramName.asChar(),
        "_values",
        "_positions");
    shaderParams.insert(positionsParamName.c_str(), positions.c_str());

    // todo: save basis here...
}

MObject ShadingNodeExporter::node() const
{
    return m_object;
}

bool ShadingNodeExporter::hasConnections(
    const MPlug&                        plug,
    const bool                          asDst,
    const bool                          asSrc) const
{
    MStatus status;
    MPlugArray inputConnections;
    plug.connectedTo(inputConnections, asDst, asSrc, &status);

    if (!status || inputConnections.length() == 0)
        return false;

    return true;
}

bool ShadingNodeExporter::hasChildrenConnections(
    const MPlug&                        plug,
    const bool                          asDst,
    const bool                          asSrc) const
{
    assert(plug.isCompound());

    if (plug.numConnectedChildren() != 0)
    {
        for (unsigned int i = 0, e = plug.numChildren(); i < e; ++i)
        {
            MStatus status;
            MPlug childPlug = plug.child(i, &status);

            if (status && hasConnections(childPlug, asDst, asSrc))
                return true;
        }
    }

    return false;
}

bool ShadingNodeExporter::hasElementConnections(
    const MPlug&                        plug,
    const bool                          asDst,
    const bool                          asSrc) const
{
    assert(plug.isArray());

    if (plug.numConnectedElements() != 0)
    {
        for (unsigned int i = 0, e = plug.numElements(); i < e; ++i)
        {
            MStatus status;
            MPlug elementPlug = plug.elementByPhysicalIndex(i, &status);

            if (status && hasConnections(elementPlug, asDst, asSrc))
                return true;
        }
    }

    return false;
}

const OSLShaderInfo& ShadingNodeExporter::getShaderInfo() const
{
    MFnDependencyNode depNodeFn(m_object);
    const OSLShaderInfo* shaderInfo = ShadingNodeRegistry::getShaderInfo(depNodeFn.typeName());
    assert(shaderInfo);

    return *shaderInfo;
}

ShadingNodeExporter* ShadingNodeExporter::findExporterForNode(
    ShadingNodeExporterMap&             exporters,
    const MObject&                      node)
{
    MFnDependencyNode depNodeFn(node);
    auto it= exporters.find(depNodeFn.name());

    if (it != exporters.end())
        return it->second;

    return nullptr;
}

ShadingNodeExporter* ShadingNodeExporter::getSrcPlugAndExporter(
    const MPlug&                        plug,
    ShadingNodeExporterMap&             exporters,
    MPlug&                              srcPlug)
{
    MStatus status;
    MPlugArray inputConnections;
    plug.connectedTo(inputConnections, true, false, &status);

    if (!status || inputConnections.length() == 0)
        return nullptr;

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
    for (unsigned int i = 0, e = plug.numChildren(); i < e; ++i)
    {
        MPlug childPlug = plug.child(i);
        if (childPlug.isConnected())
        {
            MPlug srcPlug;
            ShadingNodeExporter* srcNodeExporter = getSrcPlugAndExporter(childPlug, exporters, srcPlug);
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

    for (unsigned int i = 0, e = parentPlug.numChildren(); i < e; ++i)
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
