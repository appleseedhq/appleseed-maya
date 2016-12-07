
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

// interface header.
#include "shadingnodemetadata.h"

// Standard library headers.
#include <cstdlib>
#include <map>
#include <vector>

// appleseed.foundation headers.
#include "foundation/utility/autoreleaseptr.h"

// appleseed.renderer headers.
#include "renderer/api/shadergroup.h"

namespace asr = renderer;
namespace asf = foundation;

OSLMetadataExtractor::OSLMetadataExtractor(const foundation::Dictionary& metadata)
    : m_metadata(metadata)
{
}

bool OSLMetadataExtractor::exists(const char *key) const
{
    return m_metadata.dictionaries().exist(key);
}

bool OSLMetadataExtractor::getValue(const char *key, MString& value)
{
    if(exists(key))
    {
        const foundation::Dictionary& dict = m_metadata.dictionary(key);
        value = dict.get("value");
        return true;
    }

    return false;
}

OSLParamInfo::OSLParamInfo(const asf::Dictionary& paramInfo)
  : arrayLen(-1)
{
    paramName = paramInfo.get("name");
    mayaAttributeName = paramName;

    paramType = paramInfo.get("type");
    validDefault = paramInfo.get<bool>("validdefault");

    if(validDefault)
    {
        //TODO: get default_value here.
    }

    isOutput = paramInfo.get<bool>("isoutput");
    isClosure = paramInfo.get<bool>("isclosure");
    isStruct = paramInfo.get<bool>("isstruct");

    if(isStruct)
        structName = paramInfo.get("structname");

    isArray = paramInfo.get<bool>("isarray");

    if(isArray)
        arrayLen = paramInfo.get<int>("arraylen");
    else
        arrayLen = -1;

    if(paramInfo.dictionaries().exist("metadata"))
    {
        OSLMetadataExtractor metadata(paramInfo.dictionary("metadata"));

        metadata.getValue("label", label);
        metadata.getValue("widget", widget);

        metadata.getValue("maya_attribute_name", mayaAttributeName);
        metadata.getValue("maya_attribute_type", mayaAttributeType);
    }
}

std::ostream& operator<<(std::ostream& os, const OSLParamInfo& paramInfo)
{
    os << "Param : " << paramInfo.paramName << "\n";
    os << "  maya name      : " << paramInfo.mayaAttributeName << "\n";
    os << "  type           : " << paramInfo.paramType << "\n";
    os << "  output         : " << paramInfo.isOutput << "\n";
    os << "  valid default  : " << paramInfo.validDefault << "\n";
    os << "  closure        : " << paramInfo.isClosure << "\n";

    if(paramInfo.isStruct)
        os << "  struct name    : " << paramInfo.structName << "\n";

    if(paramInfo.isArray)
        os << "  array len      : " << paramInfo.arrayLen << "\n";

    os << std::endl;
    return os;
}

OSLShaderInfo::OSLShaderInfo()
    : typeId(0)
{
}

OSLShaderInfo::OSLShaderInfo(const asr::ShaderQuery& q)
    : typeId(0)
{
    shaderName = q.get_shader_name();
    shaderType = q.get_shader_type();
    OSLMetadataExtractor metadata(q.get_metadata());

    metadata.getValue("maya_node_name", mayaName);
    metadata.getValue("maya_classification", mayaClassification);
    metadata.getValue<unsigned int>("maya_type_id", typeId);

    paramInfo.reserve(q.get_num_params());
    for(size_t i = 0, e = q.get_num_params(); i < e; ++i)
        paramInfo.push_back(OSLParamInfo(q.get_param_info(i)));
}

const OSLParamInfo *OSLShaderInfo::findParam(const MString& mayaAttrName) const
{
    for(size_t i = 0, e = paramInfo.size(); i < e; ++i)
    {
        if(paramInfo[i].mayaAttributeName == mayaAttrName)
            return &paramInfo[i];
    }

    return 0;
}

const OSLParamInfo *OSLShaderInfo::findParam(const MPlug& plug) const
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
