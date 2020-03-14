
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
#include "shadingnode.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/logger.h"
#include "appleseedmaya/shadingnodemetadata.h"
#include "appleseedmaya/shadingnoderegistry.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.foundation headers.
#include "foundation/math/vector.h"
#include "foundation/string/string.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MFnDependencyNode.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnStringData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MObjectArray.h>
#include <maya/MRampAttribute.h>
#include "appleseedmaya/_endmayaheaders.h"

namespace asf = foundation;

namespace
{
    MObject createPointAttribute(
        MFnNumericAttribute&    numAttrFn,
        const OSLParamInfo&     p,
        MStatus&                status)
    {
        MObject attr = numAttrFn.createPoint(
            p.mayaAttributeName,
            p.mayaAttributeShortName,
            &status);

        if (p.hasDefault)
        {
            numAttrFn.setDefault(
                static_cast<float>(p.defaultValue[0]),
                static_cast<float>(p.defaultValue[1]),
                static_cast<float>(p.defaultValue[2]));
        }

        return attr;
    }

    template <typename T>
    MObject createNumericAttribute(
        MFnNumericAttribute&    numAttrFn,
        const OSLParamInfo&     p,
        MFnNumericData::Type    type,
        MStatus&                status)
    {
        MObject attr = numAttrFn.create(
            p.mayaAttributeName,
            p.mayaAttributeShortName,
            type,
            T(0),
            &status);

        if (p.hasDefault)
            numAttrFn.setDefault(static_cast<T>(p.defaultValue[0]));

        return attr;
    }

    template <>
    MObject createNumericAttribute<bool>(
        MFnNumericAttribute&    numAttrFn,
        const OSLParamInfo&     p,
        MFnNumericData::Type    type,
        MStatus&                status)
    {
        MObject attr = numAttrFn.create(
            p.mayaAttributeName,
            p.mayaAttributeShortName,
            type,
            0.0,
            &status);

        if (p.hasDefault)
            numAttrFn.setDefault(p.defaultValue[0] == 0.0 ? false : true);

        return attr;
    }

    MStatus initializeAttribute(MFnAttribute& attr, const OSLParamInfo& p)
    {
        if (p.label.length() != 0)
            attr.setNiceNameOverride(p.label);

        if (!p.mayaAttributeConnectable)
            attr.setConnectable(false);

        if (p.mayaAttributeHidden)
            attr.setHidden(true);

        return p.isOutput ?
            AttributeUtils::makeOutput(attr) :
            AttributeUtils::makeInput(attr, p.mayaAttributeKeyable);
    }

    MStatus initializeNumericAttribute(MFnNumericAttribute& attr, const OSLParamInfo& p)
    {
        if (p.hasMin)
            attr.setMin(p.minValue);

        if (p.hasMax)
            attr.setMax(p.maxValue);

        if (p.hasSoftMin)
            attr.setSoftMin(p.softMinValue);

        if (p.hasSoftMax)
            attr.setSoftMax(p.softMaxValue);

        return initializeAttribute(attr, p);
    }

    // Stores the current shader info of the shader being registered.
    const OSLShaderInfo* g_currentShaderInfo = nullptr;
}

void ShadingNode::setCurrentShaderInfo(const OSLShaderInfo* shaderInfo)
{
    g_currentShaderInfo = shaderInfo;
}

void* ShadingNode::creator()
{
    return new ShadingNode();
}

MStatus ShadingNode::initialize()
{
    #define CHECK_STATUS_AND_HANDLE_ERROR           \
        if (!status)                                \
        {                                           \
            report_error(*shaderInfo, p, status);   \
            return status;                          \
        }

    assert(g_currentShaderInfo);
    const OSLShaderInfo* shaderInfo = g_currentShaderInfo;
    g_currentShaderInfo = nullptr;

    MObjectArray inputAttributes;
    MObjectArray outputAttributes;

    // todo: lots of refactoring possibilities here...
    for (size_t i = 0, e = shaderInfo->paramInfo.size(); i < e; ++i)
    {
        const OSLParamInfo& p = shaderInfo->paramInfo[i];

        MStatus status;
        MObject attr;

        if (p.asWidget == "ramp")
        {
            if (p.paramType == "color[]")
            {
                attr = MRampAttribute::createColorRamp(
                    p.mayaAttributeName,
                    p.mayaAttributeShortName);
            }
            else if (p.paramType == "float[]")
            {
                attr = MRampAttribute::createCurveRamp(
                    p.mayaAttributeName,
                    p.mayaAttributeShortName);
            }
            else
            {
                RENDERER_LOG_WARNING(
                    "Error while initializing node %s, param %s, error = %s",
                    shaderInfo->shaderName.asChar(),
                    p.paramName.asChar(),
                    status.errorString().asChar());
                return MS::kFailure;
            }
        }
        else if (p.asWidget == "ramp_positions")
        {
            // Ramp positions are part of the RampAttribute.
            continue;
        }
        else if (p.asWidget == "ramp_basis")
        {
            // Ramp basis is part of the RampAttribute.
            continue;
        }
        else if (p.paramType == "color")
        {
            MFnNumericAttribute numAttrFn;
            attr = numAttrFn.createColor(
                p.mayaAttributeName,
                p.mayaAttributeShortName,
                &status);
            CHECK_STATUS_AND_HANDLE_ERROR

            numAttrFn.setUsedAsColor(true);

            if (p.hasDefault)
            {
                numAttrFn.setDefault(
                    static_cast<float>(p.defaultValue[0]),
                    static_cast<float>(p.defaultValue[1]),
                    static_cast<float>(p.defaultValue[2]));
            }

            status = initializeAttribute(numAttrFn, p);
            CHECK_STATUS_AND_HANDLE_ERROR
        }
        else if (p.paramType == "float")
        {
            MFnNumericAttribute numAttrFn;
            attr = createNumericAttribute<float>(numAttrFn, p, MFnNumericData::kFloat, status);
            CHECK_STATUS_AND_HANDLE_ERROR

            status = initializeNumericAttribute(numAttrFn, p);
            CHECK_STATUS_AND_HANDLE_ERROR
        }
        else if (p.paramType == "int")
        {
            // Check to see if we need to create an int, bool or enum
            if (p.widget == "mapper")
            {
                if (p.options.length() != 0)
                {
                    std::vector<std::string> fields;
                    asf::tokenize(p.options.asChar(), "|", fields);

                    int defaultValue = 0;
                    if (p.hasDefault)
                        defaultValue = static_cast<int>(p.defaultValue[0]);

                    MFnEnumAttribute enumAttrFn;
                    attr = enumAttrFn.create(
                        p.mayaAttributeName,
                        p.mayaAttributeShortName,
                        defaultValue,
                        &status);
                    CHECK_STATUS_AND_HANDLE_ERROR

                    std::vector<std::string> subfields;
                    for (size_t i = 0, e = fields.size(); i < e; ++i)
                    {
                        subfields.clear();
                        asf::tokenize(fields[i].c_str(), ":", subfields);
                        status = enumAttrFn.addField(
                            subfields[0].c_str(),
                            asf::from_string<int>(subfields[1]));
                        CHECK_STATUS_AND_HANDLE_ERROR
                    }

                    status = initializeAttribute(enumAttrFn, p);
                    CHECK_STATUS_AND_HANDLE_ERROR
                }
                else
                {
                    // todo: what here...?
                }
            }
            else if (p.widget == "checkBox")
            {
                MFnNumericAttribute numAttrFn;
                attr = createNumericAttribute<bool>(numAttrFn, p, MFnNumericData::kBoolean, status);
                CHECK_STATUS_AND_HANDLE_ERROR

                status = initializeAttribute(numAttrFn, p);
                CHECK_STATUS_AND_HANDLE_ERROR
            }
            else // normal int attribute.
            {
                MFnNumericAttribute numAttrFn;
                attr = createNumericAttribute<int>(numAttrFn, p, MFnNumericData::kInt, status);
                CHECK_STATUS_AND_HANDLE_ERROR

                status = initializeNumericAttribute(numAttrFn, p);
                CHECK_STATUS_AND_HANDLE_ERROR
            }
        }
        else if (p.paramType == "matrix")
        {
            MFnMatrixAttribute matrixAttrFn;
            attr = matrixAttrFn.create(
                p.mayaAttributeName,
                p.mayaAttributeShortName,
                MFnMatrixAttribute::kFloat,
                &status);
            CHECK_STATUS_AND_HANDLE_ERROR

            initializeAttribute(matrixAttrFn, p);
            CHECK_STATUS_AND_HANDLE_ERROR
        }
        else if (p.paramType == "normal")
        {
            MFnNumericAttribute numAttrFn;
            attr = createPointAttribute(numAttrFn, p, status);
            CHECK_STATUS_AND_HANDLE_ERROR

            status = initializeAttribute(numAttrFn, p);
            CHECK_STATUS_AND_HANDLE_ERROR
        }
        else if (p.paramType == "point")
        {
            MFnNumericAttribute numAttrFn;
            attr = createPointAttribute(numAttrFn, p, status);
            CHECK_STATUS_AND_HANDLE_ERROR

            status = initializeAttribute(numAttrFn, p);
            CHECK_STATUS_AND_HANDLE_ERROR
        }
        else if (p.paramType == "pointer") // closure color
        {
            MFnNumericAttribute numAttrFn;
            attr = numAttrFn.createColor(
                p.mayaAttributeName,
                p.mayaAttributeShortName,
                &status);
            CHECK_STATUS_AND_HANDLE_ERROR

            status = initializeAttribute(numAttrFn, p);
            CHECK_STATUS_AND_HANDLE_ERROR
        }
        else if (p.paramType == "string")
        {
            // Check to see if we need to create a string or an enum
            if (p.widget == "popup")
            {
                if (p.options.length() != 0)
                {
                    std::vector<std::string> fields;
                    asf::tokenize(p.options.asChar(), "|", fields);

                    size_t defaultValue = 0;
                    if (p.hasDefault)
                    {
                        for (size_t i = 0, e = fields.size(); i < e; ++i)
                        {
                            if (p.defaultStringValue == fields[i].c_str())
                            {
                                defaultValue = i;
                                break;
                            }
                        }
                    }

                    MFnEnumAttribute enumAttrFn;
                    attr = enumAttrFn.create(
                        p.mayaAttributeName,
                        p.mayaAttributeShortName,
                        static_cast<short>(defaultValue),
                        &status);
                    CHECK_STATUS_AND_HANDLE_ERROR

                    for (size_t i = 0, e = fields.size(); i < e; ++i)
                        enumAttrFn.addField(fields[i].c_str(), static_cast<short>(i));

                    status = initializeAttribute(enumAttrFn, p);
                    CHECK_STATUS_AND_HANDLE_ERROR
                }
                else
                {
                    // todo: what here...?
                }
            }
            else
            {
                MFnTypedAttribute typedAttrFn;
                attr = typedAttrFn.create(
                    p.mayaAttributeName,
                    p.mayaAttributeShortName,
                    MFnData::kString,
                    &status);
                CHECK_STATUS_AND_HANDLE_ERROR

                if (p.widget == "filename")
                    typedAttrFn.setUsedAsFilename(true);

                if (p.hasDefault)
                {
                    MFnStringData stringDataFn;
                    MObject defaultData = stringDataFn.create(p.defaultStringValue);
                    typedAttrFn.setDefault(defaultData);
                }

                status = initializeAttribute(typedAttrFn, p);
                CHECK_STATUS_AND_HANDLE_ERROR
            }
        }
        else if (p.paramType == "vector")
        {
            MFnNumericAttribute numAttrFn;
            attr = createPointAttribute(numAttrFn, p, status);
            CHECK_STATUS_AND_HANDLE_ERROR

            status = initializeAttribute(numAttrFn, p);
            CHECK_STATUS_AND_HANDLE_ERROR
        }
        else if (p.paramType == "float[2]")
        {
            MFnNumericAttribute numAttrFn;

            // Special UV cases
            if (p.mayaAttributeName == "uvCoord")
            {
                MObject child1 = numAttrFn.create("uCoord", "u", MFnNumericData::kFloat);
                MObject child2 = numAttrFn.create("vCoord", "v", MFnNumericData::kFloat);
                attr = numAttrFn.create("uvCoord", "uv", child1, child2);
            }
            else if (p.mayaAttributeName == "uvFilterSize")
            {
                MObject child1 = numAttrFn.create("uvFilterSizeX", "fsx", MFnNumericData::kFloat);
                MObject child2 = numAttrFn.create("uvFilterSizeY", "fsy", MFnNumericData::kFloat);
                attr = numAttrFn.create("uvFilterSize", "fs", child1, child2);
            }
            else if (p.mayaAttributeName == "outUV")
            {
                MObject child1 = numAttrFn.create("outU", "ou", MFnNumericData::kFloat);
                MObject child2 = numAttrFn.create("outV", "ov", MFnNumericData::kFloat);
                attr = numAttrFn.create("outUV", "o", child1, child2);
            }
            else
            {
                MObject child1 = numAttrFn.create(
                    p.mayaAttributeName + "X",
                    p.mayaAttributeShortName + "x",
                    MFnNumericData::kFloat,
                    p.defaultValue[0]);

                MObject child2 = numAttrFn.create(
                    p.mayaAttributeName + "Y",
                    p.mayaAttributeShortName + "y",
                    MFnNumericData::kFloat,
                    p.defaultValue[1]);

                attr = numAttrFn.create(p.mayaAttributeName, p.mayaAttributeShortName, child1, child2);
            }

            status = initializeAttribute(numAttrFn, p);
            CHECK_STATUS_AND_HANDLE_ERROR
        }
        else
        {
            RENDERER_LOG_WARNING(
                "Ignoring param %s of shader %s",
                p.paramName.asChar(),
                shaderInfo->shaderName.asChar());
            continue;
        }

        if (!attr.isNull())
        {
            if (p.isOutput)
                outputAttributes.append(attr);
            else
                inputAttributes.append(attr);

            status = addAttribute(attr);
            CHECK_STATUS_AND_HANDLE_ERROR
        }
    }

    #undef CHECK_STATUS_AND_HANDLE_ERROR

    // Set dependencies between input and output attributes.
    // This is needed for swatches to update correctly.
    for (unsigned int i = 0 , ie = inputAttributes.length() ; i < ie ; ++i)
    {
        for (unsigned int j = 0 , je = outputAttributes.length() ; j < je ; ++j)
            attributeAffects(inputAttributes[i], outputAttributes[j]);
    }

    return MS::kSuccess;
}

ShadingNode::ShadingNode()
{
}

void ShadingNode::postConstructor()
{
    MPxNode::postConstructor();
    setMPSafe(true);

    setExistWithoutInConnections(true);
    setExistWithoutOutConnections(true);
}

void ShadingNode::report_error(
    const OSLShaderInfo&    shaderInfo,
    const OSLParamInfo&     paramInfo,
    MStatus&                status)
{
    RENDERER_LOG_WARNING(
        "Error while initializing node %s, param %s, error = %s",
        shaderInfo.shaderName.asChar(),
        paramInfo.paramName.asChar(),
        status.errorString().asChar());
}
