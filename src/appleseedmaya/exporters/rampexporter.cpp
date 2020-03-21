
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
#include "rampexporter.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exporters/exporterfactory.h"
#include "appleseedmaya/ramputils.h"
#include "appleseedmaya/shadingnodemetadata.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/utility.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MFnDependencyNode.h>
#include "appleseedmaya/_endmayaheaders.h"

// Standard headers.
#include <algorithm>
#include <sstream>
#include <vector>

namespace asf = foundation;
namespace asr = renderer;

void RampExporter::registerExporter()
{
    NodeExporterFactory::registerShadingNodeExporter(
        "ramp",
        &RampExporter::create);
}

ShadingNodeExporter* RampExporter::create(
    const MObject&      object,
    asr::ShaderGroup&   shaderGroup)
{
    return new RampExporter(object, shaderGroup);
}

RampExporter::RampExporter(
    const MObject&      object,
    asr::ShaderGroup&   shaderGroup)
  : ShadingNodeExporter(object, shaderGroup)
{
}

void RampExporter::exportParameterValue(
    const MPlug&        plug,
    const OSLParamInfo& paramInfo,
    asr::ParamArray&    shaderParams) const
{
    MFnDependencyNode depNodeFn(node());
    MStatus status;

    if (paramInfo.paramName == "in_position")
    {
        MPlug plug = depNodeFn.findPlug("colorEntryList", /*wantNetworkedPlug=*/ false, &status);

        std::vector<RampEntry<MColor>> rampColors;
        rampColors.reserve(plug.numElements());

        for (unsigned int i = 0, e = plug.numElements(); i < e; ++i)
        {
            MPlug entry = plug.elementByPhysicalIndex(i);
            MPlug position = entry.child(0);
            MPlug color = entry.child(1);

            float p;
            AttributeUtils::get(position, p);

            MColor c;
            AttributeUtils::get(color, c);

            rampColors.push_back(RampEntry<MColor>(i, p, c));
        }

        plug = depNodeFn.findPlug("type", /*wantNetworkedPlug=*/ false, &status);
        int rampType;
        AttributeUtils::get(plug, rampType);

        // Check if the type is four corner.
        if (rampType == 7)
        {
            // Fill with black if we have less than 4 elements.
            while (rampColors.size() < 4)
            {
                rampColors.push_back(
                    RampEntry<MColor>(
                        static_cast<int>(rampColors.size()),
                        1.0f,
                        MColor(0.0f, 0.0f, 0.0f)));
            }
        }
        else
        {
            // Sort the ramp entries.
            std::sort(rampColors.begin(), rampColors.end());
        }

        std::string values;
        std::string positions;
        serializeRamp(rampColors, values, positions);

        shaderParams.insert("in_position", positions.c_str());
        shaderParams.insert("in_color"   , values.c_str());
    }
    else if (paramInfo.paramName == "in_color")
    {
        // We save the colors at the same time we save the positions.
    }
    else
    {
        ShadingNodeExporter::exportParameterValue(
            plug,
            paramInfo,
            shaderParams);
    }
}
