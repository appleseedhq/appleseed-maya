
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016-2018 Esteban Tovagliari, The appleseedhq Organization
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
#include "shadingnodemetadata.h"

// appleseed.maya headers.
#include "appleseedmaya/logger.h"

// appleseed.renderer headers.
#include "renderer/api/shadergroup.h"

// appleseed.foundation headers.
#include "foundation/utility/autoreleaseptr.h"
#include "foundation/utility/iostreamop.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MPlug.h>
#include "appleseedmaya/_endmayaheaders.h"

// Standard headers.
#include <cstdlib>
#include <map>
#include <vector>

namespace asr = renderer;
namespace asf = foundation;

OSLMetadataExtractor::OSLMetadataExtractor(const foundation::Dictionary& metadata)
    : m_metadata(metadata)
{
}

bool OSLMetadataExtractor::exists(const char* key) const
{
    return m_metadata.dictionaries().exist(key);
}

bool OSLMetadataExtractor::getValue(const char* key, MString& value)
{
    if (exists(key))
    {
        const foundation::Dictionary& dict = m_metadata.dictionary(key);
        value = dict.get("value");
        return true;
    }

    return false;
}

bool OSLMetadataExtractor::getValue(const char* key, bool& value)
{
    int tmp;
    if (getValue(key, tmp))
    {
        value = (tmp != 0);
        return true;
    }

    return false;
}

namespace
{
    void getFloat3Default(const asf::Dictionary& paramInfo, std::vector<double>& defaultValue)
    {
        const asf::Vector3f v = paramInfo.get<asf::Vector3f>("default");
        defaultValue.push_back(v[0]);
        defaultValue.push_back(v[1]);
        defaultValue.push_back(v[2]);
    }
}

OSLParamInfo::OSLParamInfo(const asf::Dictionary& paramInfo)
  : arrayLen(-1)
  , lockGeom(true)
  , hasDefault(false)
  , divider(false)
{
    paramName = paramInfo.get("name");
    mayaAttributeName = paramName;
    mayaAttributeShortName = paramName;
    mayaAttributeConnectable = true;
    mayaAttributeHidden = false;
    mayaAttributeKeyable = true;

    paramType = paramInfo.get("type");
    validDefault = paramInfo.get<bool>("validdefault");

    // todo: lots of refactoring possibilities here...
    if (validDefault && lockGeom)
    {
        if (paramInfo.strings().exist("default"))
        {
            if (paramType == "color")
            {
                getFloat3Default(paramInfo, defaultValue);
                hasDefault = true;
            }
            else if (paramType == "float")
            {
                defaultValue.push_back(paramInfo.get<float>("default"));
                hasDefault = true;
            }
            else if (paramType == "float[2]")
            {
                const asf::Vector2f v = paramInfo.get<asf::Vector2f>("default");
                defaultValue.push_back(v[0]);
                defaultValue.push_back(v[1]);

                hasDefault = true;
            }
            else if (paramType == "int")
            {
                defaultValue.push_back(paramInfo.get<int>("default"));
                hasDefault = true;
            }
            if (paramType == "normal")
            {
                getFloat3Default(paramInfo, defaultValue);
                hasDefault = true;
            }
            if (paramType == "point")
            {
                getFloat3Default(paramInfo, defaultValue);
                hasDefault = true;
            }
            else if (paramType == "string")
            {
                defaultStringValue = paramInfo.get("default");
                hasDefault = true;
            }
            else if (paramType == "vector")
            {
                getFloat3Default(paramInfo, defaultValue);
                hasDefault = true;
            }
        }
    }

    isOutput = paramInfo.get<bool>("isoutput");
    isClosure = paramInfo.get<bool>("isclosure");
    isStruct = paramInfo.get<bool>("isstruct");

    if (isStruct)
        structName = paramInfo.get("structname");

    isArray = paramInfo.get<bool>("isarray");

    if (isArray)
        arrayLen = paramInfo.get<int>("arraylen");
    else
        arrayLen = -1;

    if (paramInfo.dictionaries().exist("metadata"))
    {
        OSLMetadataExtractor metadata(paramInfo.dictionary("metadata"));

        metadata.getValue("lockgeom", lockGeom);
        metadata.getValue("units", units);
        metadata.getValue("page", page);
        metadata.getValue("label", label);
        metadata.getValue("widget", widget);
        metadata.getValue("options", options);
        metadata.getValue("help", help);
        hasMin = metadata.getValue("min", minValue);
        hasMax = metadata.getValue("max", maxValue);
        hasSoftMin = metadata.getValue("softmin", softMinValue);
        hasSoftMax = metadata.getValue("softmax", softMaxValue);
        metadata.getValue("divider", divider);

        metadata.getValue("as_widget", asWidget);

        metadata.getValue("as_maya_attribute_name", mayaAttributeName);
        metadata.getValue("as_maya_attribute_short_name", mayaAttributeShortName);
        metadata.getValue("as_maya_attribute_connectable", mayaAttributeConnectable);
        metadata.getValue("as_maya_attribute_hidden", mayaAttributeHidden);
        metadata.getValue("as_maya_attribute_keyable", mayaAttributeKeyable);
    }
}

std::ostream& operator<<(std::ostream& os, const OSLParamInfo& paramInfo)
{
    os << "Param : " << paramInfo.paramName << "\n";
    os << "  maya name      : " << paramInfo.mayaAttributeName << "\n";
    os << "  type           : " << paramInfo.paramType << "\n";
    os << "  widget         : " << paramInfo.widget << "\n";
    os << "  output         : " << paramInfo.isOutput << "\n";
    os << "  valid default  : " << paramInfo.validDefault << "\n";
    os << "  closure        : " << paramInfo.isClosure << "\n";

    if (paramInfo.isStruct)
        os << "  struct name    : " << paramInfo.structName << "\n";

    if (paramInfo.isArray)
        os << "  array len      : " << paramInfo.arrayLen << "\n";

    os << std::endl;
    return os;
}

OSLShaderInfo::OSLShaderInfo()
    : typeId(0)
{
}

OSLShaderInfo::OSLShaderInfo(
    const asr::ShaderQuery&     q,
    const MString&              filename)
    : typeId(0)
{
    shaderName = q.get_shader_name();
    shaderType = q.get_shader_type();
    shaderFileName = filename;
    OSLMetadataExtractor metadata(q.get_metadata());

    metadata.getValue("as_node_name", mayaName);
    metadata.getValue("as_maya_classification", mayaClassification);
    metadata.getValue<unsigned int>("as_maya_type_id", typeId);
    metadata.getValue("URL", shaderHelpURL);

    paramInfo.reserve(q.get_param_count());
    for (size_t i = 0, e = q.get_param_count(); i < e; ++i)
        paramInfo.push_back(OSLParamInfo(q.get_param_info(i)));

    // Apply some defaults.

    // If the shader is a custom node, we can default its name to the shader name.
    if (typeId != 0 && mayaName.length() == 0)
        mayaName = shaderName;
}

const OSLParamInfo* OSLShaderInfo::findParam(const MString& mayaAttrName) const
{
    for (size_t i = 0, e = paramInfo.size(); i < e; ++i)
    {
        if (paramInfo[i].mayaAttributeName == mayaAttrName)
            return &paramInfo[i];
    }

    return nullptr;
}

const OSLParamInfo* OSLShaderInfo::findParam(const MPlug& plug) const
{
    MStatus status;
    const MString attrName =
        plug.partialName(
            false,
            false,
            false,
            false,
            false,
            true,   // use long names.
            &status);
    return findParam(attrName);
}
