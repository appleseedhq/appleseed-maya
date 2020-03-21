
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
#include "place3dtextureexporter.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exporters/exporterfactory.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/utility.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MDagPath.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MMatrix.h>
#include "appleseedmaya/_endmayaheaders.h"

// Standard headers.
#include <sstream>

namespace asf = foundation;
namespace asr = renderer;

void Place3dTextureExporter::registerExporter()
{
    NodeExporterFactory::registerShadingNodeExporter(
        "place3dTexture",
        &Place3dTextureExporter::create);
}

ShadingNodeExporter* Place3dTextureExporter::create(
    const MObject&           object,
    asr::ShaderGroup&        shaderGroup)
{
    return new Place3dTextureExporter(object, shaderGroup);
}

Place3dTextureExporter::Place3dTextureExporter(
    const MObject&           object,
    asr::ShaderGroup&        shaderGroup)
  : ShadingNodeExporter(object, shaderGroup)
{
}

void Place3dTextureExporter::exportShaderParameters(
    const OSLShaderInfo&     shaderInfo,
    renderer::ParamArray&    shaderParams) const
{
    // Save the place3dTexture matrix.
    MDagPath dagPath;
    MDagPath::getAPathTo(node(), dagPath);
    MMatrix matrixValue = dagPath.inclusiveMatrixInverse();

    std::stringstream ss;
    ss << "matrix ";
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            ss << matrixValue[i][j] << " ";
    shaderParams.insert("inclusiveMatrixInverse", ss.str().c_str());

    // Handle the rest of the parameters.
    ShadingNodeExporter::exportShaderParameters(
        shaderInfo,
        shaderParams);
}

bool Place3dTextureExporter::layerAndParamNameFromPlug(
    const MPlug&             plug,
    MString&                 layerName,
    MString&                 paramName)
{
    if (plug.isElement())
    {
        MStatus status;
        MPlug parentPlug = plug.array(&status);
        const MString parentPlugName =
            parentPlug.partialName(
                false,
                false,
                false,
                false,
                false,
                true,   // use long names.
                &status);

        if (parentPlugName == "worldInverseMatrix")
        {
            MFnDependencyNode depNodeFn(node());
            layerName = depNodeFn.name();
            paramName = "out_worldInverseMatrix";
            return true;
        }
    }

    return ShadingNodeExporter::layerAndParamNameFromPlug(plug, layerName, paramName);
}
