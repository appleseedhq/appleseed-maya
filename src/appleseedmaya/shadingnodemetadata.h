
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

#ifndef APPLESEED_MAYA_SHADING_NODE_METADATA_H
#define APPLESEED_MAYA_SHADING_NODE_METADATA_H

// Standard headers.
#include <vector>

// Maya headers.
#include <maya/MString.h>

// appleseed.foundation headers.
#include "foundation/utility/containers/dictionary.h"

// appleseed.maya headers.
#include "appleseedmaya/utils.h"

// Forward declarations.
namespace renderer { class ShaderQuery; }


class OSLMetadataExtractor
  : NonCopyable
{
  public:
    explicit OSLMetadataExtractor(const foundation::Dictionary& metadata);

    bool exists(const char *key) const;

    bool getValue(const char *key, MString& value);

    template <typename T>
    bool getValue(const char *key, T& value) const
    {
        if(exists(key))
        {
            const foundation::Dictionary& dict = m_metadata.dictionary(key);
            value = dict.get<T>("value");
            return true;
        }

        return false;
    }

  private:
    const foundation::Dictionary& m_metadata;
};

class OSLParamInfo
{
  public:
    explicit OSLParamInfo(const foundation::Dictionary& paramInfo);

    // Query info.
    MString paramName;
    MString paramType;
    bool validDefault;
    //T default_value
    bool isOutput;
    bool isClosure;
    bool isStruct;
    MString structName;
    bool isArray;
    int arrayLen;

    // Standard metadata info.

    // More standard metadata options...

    // appleseedMaya custom metadata.
    MString mayaAttributeName;
};

class OSLShaderInfo
{
  public:
    OSLShaderInfo();

    explicit OSLShaderInfo(const renderer::ShaderQuery& q);

    MString shaderName;
    MString shaderType;

    MString mayaName;
    MString mayaClassification;
    unsigned int typeId;

    std::vector<OSLParamInfo> paramInfo;
};

#endif  // !APPLESEED_MAYA_SHADING_NODE_METADATA_H
