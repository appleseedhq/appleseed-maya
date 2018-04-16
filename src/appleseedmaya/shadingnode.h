
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

#ifndef APPLESEED_MAYA_SHADINGNODE_H
#define APPLESEED_MAYA_SHADINGNODE_H

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MPxNode.h>
#include "appleseedmaya/_endmayaheaders.h"

// Forward declarations.
class OSLParamInfo;
class OSLShaderInfo;

class ShadingNode
  : public MPxNode
{
  public:
    // Set the shader info of the current shader being registered.
    static void setCurrentShaderInfo(const OSLShaderInfo* shaderInfo);

    static void* creator();
    static MStatus initialize();

    ShadingNode();

    void postConstructor() override;

  private:
    static void report_error(
        const OSLShaderInfo&    shaderInfo,
        const OSLParamInfo&     paramInfo,
        MStatus&                status);
};

#endif  // !APPLESEED_MAYA_SHADINGNODE_H
