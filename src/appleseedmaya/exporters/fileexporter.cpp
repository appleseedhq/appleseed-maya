
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2017-2019 Esteban Tovagliari, The appleseedhq Organization
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
#include "fileexporter.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exporters/exporterfactory.h"
#include "appleseedmaya/shadingnodemetadata.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/utility.h"

// appleseed.foundation headers.
#include "foundation/string/string.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MRenderUtil.h>
#include "appleseedmaya/_endmayaheaders.h"

namespace asf = foundation;
namespace asr = renderer;

void FileExporter::registerExporter()
{
    NodeExporterFactory::registerShadingNodeExporter(
        "file",
        &FileExporter::create);
}

ShadingNodeExporter* FileExporter::create(
    const MObject&      object,
    asr::ShaderGroup&   shaderGroup)
{
    return new FileExporter(object, shaderGroup);
}

FileExporter::FileExporter(
    const MObject&      object,
    asr::ShaderGroup&   shaderGroup)
  : ShadingNodeExporter(object, shaderGroup)
{
}

void FileExporter::exportParameterValue(
    const MPlug&        plug,
    const OSLParamInfo& paramInfo,
    asr::ParamArray&    shaderParams) const
{
    if (paramInfo.paramName == "in_fileTextureName")
    {
        MStatus status;
        const MString textureFileName =
            MRenderUtil::exactFileTextureName(node(), &status);

        const MString value = MString("string ") + textureFileName;
        shaderParams.insert("in_fileTextureName", value.asChar());
        return;
    }

    return ShadingNodeExporter::exportParameterValue(
        plug, paramInfo, shaderParams);
}
