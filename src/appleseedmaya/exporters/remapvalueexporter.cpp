
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2017-2019 Luis Barrancos, The appleseedhq Organization
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
#include "remapvalueexporter.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exporters/exporterfactory.h"
#include "appleseedmaya/shadingnodemetadata.h"

// Build options header.
//#include "foundation/core/buildoptions.h"

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

namespace
{
    struct RemapValueEntry
    {
        RemapValueEntry(float pos, float val, int vin)
          : m_pos(pos)
          , m_value(val)
          , m_interp(vin)
        {
        }

        bool operator<(const RemapValueEntry& other) const
        {
            return m_pos < other.m_pos;
        }

        float   m_pos;
        float   m_value;
        int     m_interp;
    };

    struct RemapColorsEntry
    {
        RemapColorsEntry(float pos, const MColor& col, int cin)
          : m_pos(pos)
          , m_color(col)
          , m_interp(cin)
        {
        }

        bool operator<(const RemapColorsEntry& other) const
        {
            return m_pos < other.m_pos;
        }

        float   m_pos;
        MColor  m_color;
        int     m_interp;
    };
}

void RemapValueExporter::registerExporter()
{
    NodeExporterFactory::registerShadingNodeExporter(
        "remapValue",
        &RemapValueExporter::create);
}

ShadingNodeExporter* RemapValueExporter::create(
    const MObject&      object,
    asr::ShaderGroup&   shaderGroup)
{
    return new RemapValueExporter(object, shaderGroup);
}

RemapValueExporter::RemapValueExporter(
    const MObject&      object,
    asr::ShaderGroup&   shaderGroup)
  : ShadingNodeExporter(object, shaderGroup)
{
}

void RemapValueExporter::exportParameterValue(
    const MPlug&        plug,
    const OSLParamInfo& paramInfo,
    asr::ParamArray&    shaderParams) const
{
    MFnDependencyNode depNodeFn(node());
    MStatus status;

    if (paramInfo.paramName == "in_value_Position")
    {
        MPlug plug = depNodeFn.findPlug("value", /*wantNetworkedPlug=*/ false, &status);

        std::vector<RemapValueEntry> remapValue;
        remapValue.reserve(plug.numElements());

        for (unsigned int i = 0, e = plug.numElements(); i < e; ++i)
        {
            MPlug entry = plug.elementByPhysicalIndex(i);
            MPlug position = entry.child(0);
            MPlug value = entry.child(1);
            MPlug interp = entry.child(2);

            float p;
            AttributeUtils::get(position, p);

            float v;
            AttributeUtils::get(value, v);

            int in;
            AttributeUtils::get(interp, in);

            remapValue.push_back(RemapValueEntry(p, v, in));
        }

        std::stringstream ssp;
        ssp << "float[] ";

        std::stringstream ssv;
        ssv << "float[] ";

        std::stringstream ssi;
        ssi << "int[] ";

        for (size_t i = 0, e = remapValue.size(); i < e; ++i)
        {
            ssp << remapValue[i].m_pos << " ";
            ssv << remapValue[i].m_value << " ";
            ssi << remapValue[i].m_interp << " ";
        }

        shaderParams.insert("in_value_Position"     , ssp.str().c_str());
        shaderParams.insert("in_value_FloatValue"   , ssv.str().c_str());
        shaderParams.insert("in_value_Interp"       , ssi.str().c_str());
    }
    else if (paramInfo.paramName == "in_color_Position")
    {
        MPlug plug = depNodeFn.findPlug("color", /*wantNetworkedPlug=*/ false, &status);

        std::vector<RemapColorsEntry> remapColors;
        remapColors.reserve(plug.numElements());

        for (unsigned int i = 0, e = plug.numElements(); i < e; ++i)
        {
            MPlug entry = plug.elementByPhysicalIndex(i);
            MPlug position = entry.child(0);
            MPlug color = entry.child(1);
            MPlug interp = entry.child(2);

            float p;
            AttributeUtils::get(position, p);

            MColor c;
            AttributeUtils::get(color, c);

            int in;
            AttributeUtils::get(interp, in);

            remapColors.push_back(RemapColorsEntry(p, c, in));
        }

        std::stringstream ssp;
        ssp << "float[] ";

        std::stringstream ssc;
        ssc<< "color[] ";

        std::stringstream ssi;
        ssi << "int[] ";

        for (size_t i = 0, e = remapColors.size(); i < e; ++i)
        {
            ssp << remapColors[i].m_pos << " ";
            ssc << remapColors[i].m_color.r << " " << remapColors[i].m_color.g << " " << remapColors[i].m_color.b << " ";
            ssi << remapColors[i].m_interp << " ";
        }

        shaderParams.insert("in_color_Position"     , ssp.str().c_str());
        shaderParams.insert("in_color_Color"   , ssc.str().c_str());
        shaderParams.insert("in_color_Interp"       , ssi.str().c_str());
    }
    else if (
        paramInfo.paramName == "in_value_FloatValue" ||
        paramInfo.paramName == "in_value_Interp" ||
        paramInfo.paramName == "in_color_Color" ||
        paramInfo.paramName == "in_color_Interp")
    {
        ; // Value, interpolation modes saved with positions
    }
    else
    {
        ShadingNodeExporter::exportParameterValue(
            plug,
            paramInfo,
            shaderParams);
    }
}
