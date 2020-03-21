
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
#include "mandelbrotexporter.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exporters/exporterfactory.h"
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

namespace
{
    struct MandelbrotColorsEntry
    {
        MandelbrotColorsEntry(float pos, const MColor& col, int interp)
          : m_pos(pos)
          , m_col(col)
          , m_interp(interp)
        {
        }

        bool operator<(const MandelbrotColorsEntry& other) const
        {
            return m_pos < other.m_pos;
        }

        float   m_pos;
        MColor  m_col;
        int     m_interp;
    };

    struct MandelbrotValuesEntry
    {
        MandelbrotValuesEntry(float pos, float val, int interp)
          : m_pos(pos)
          , m_val(val)
          , m_interp(interp)
        {
        }

        bool operator<(const MandelbrotValuesEntry& other) const
        {
            return m_pos < other.m_pos;
        }

        float   m_pos;
        float   m_val;
        int     m_interp;
    };
}

void MandelbrotExporter::registerExporter()
{
    NodeExporterFactory::registerShadingNodeExporter(
        "mandelbrot",
        &MandelbrotExporter::create);
}

ShadingNodeExporter* MandelbrotExporter::create(
    const MObject&      object,
    asr::ShaderGroup&   shaderGroup)
{
    return new MandelbrotExporter(object, shaderGroup);
}

MandelbrotExporter::MandelbrotExporter(
    const MObject&      object,
    asr::ShaderGroup&   shaderGroup)
  : ShadingNodeExporter(object, shaderGroup)
{
}

void MandelbrotExporter::exportParameterValue(
    const MPlug&        plug,
    const OSLParamInfo& paramInfo,
    asr::ParamArray&    shaderParams) const
{
    MFnDependencyNode depNodeFn(node());
    MStatus status;

    if (paramInfo.paramName == "in_color_Position")
    {
        MPlug plug = depNodeFn.findPlug("color", /*wantNetworkedPlug=*/ false, &status);

        std::vector<MandelbrotColorsEntry> mandelbrotColors;
        mandelbrotColors.reserve(plug.numElements());

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

            mandelbrotColors.push_back(MandelbrotColorsEntry(p, c, in));
        }

        std::sort(mandelbrotColors.begin(), mandelbrotColors.end());

        std::stringstream ssp;
        ssp << "float[] ";

        std::stringstream ssc;
        ssc << "color[] ";

        std::stringstream ssi;
        ssi << "int[] ";

        for (size_t i = 0, e = mandelbrotColors.size(); i < e; ++i)
        {
            ssp << mandelbrotColors[i].m_pos << " ";
            ssc << mandelbrotColors[i].m_col.r << " " << mandelbrotColors[i].m_col.g << " " << mandelbrotColors[i].m_col.b << " ";
            ssi << mandelbrotColors[i].m_interp << " ";
        }

        shaderParams.insert("in_color_Position", ssp.str().c_str());
        shaderParams.insert("in_color_Color"   , ssc.str().c_str());
        shaderParams.insert("in_color_Interp"  , ssi.str().c_str());
    }
    else if (paramInfo.paramName == "in_value_Position")
    {
        MPlug plug = depNodeFn.findPlug("value", /*wantNetworkedPlug=*/ false, &status);

        std::vector<MandelbrotValuesEntry> mandelbrotValues;
        mandelbrotValues.reserve(plug.numElements());

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

            mandelbrotValues.push_back(MandelbrotValuesEntry(p, v, in));
        }

        std::sort(mandelbrotValues.begin(), mandelbrotValues.end());

        std::stringstream ssp;
        ssp << "float[] ";

        std::stringstream ssv;
        ssv << "float[] ";

        std::stringstream ssi;
        ssi << "int[] ";

        for (size_t i = 0, e = mandelbrotValues.size(); i < e; ++i)
        {
            ssp << mandelbrotValues[i].m_pos << " ";
            ssv << mandelbrotValues[i].m_val << " ";
            ssi << mandelbrotValues[i].m_interp << " ";
        }

        shaderParams.insert("in_value_Position", ssp.str().c_str());
        shaderParams.insert("in_value_FloatValue", ssv.str().c_str());
        shaderParams.insert("in_value_Interp"  , ssi.str().c_str());
    }
    else if (paramInfo.paramName == "in_color_Color" ||
             paramInfo.paramName == "in_color_Interp"||
             paramInfo.paramName == "in_value_FloatValue" ||
             paramInfo.paramName == "in_value_Interp")
    {
        // We save the colors and values at the same time we save the positions.
    }
    else
    {
        ShadingNodeExporter::exportParameterValue(
            plug,
            paramInfo,
            shaderParams);
    }
}
