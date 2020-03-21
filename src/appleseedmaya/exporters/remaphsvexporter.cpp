
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
#include "remaphsvexporter.h"

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
    struct RemapHsvEntry
    {
        RemapHsvEntry(float pos, float val, int in)
          : m_pos(pos)
          , m_value(val)
          , m_interp(in)
        {
        }

        bool operator<(const RemapHsvEntry& other) const
        {
            return m_pos < other.m_pos;
        }

        float   m_pos;
        float   m_value;
        int     m_interp;
    };
}

void RemapHsvExporter::registerExporter()
{
    NodeExporterFactory::registerShadingNodeExporter(
        "remapHsv",
        &RemapHsvExporter::create);
}

ShadingNodeExporter* RemapHsvExporter::create(
    const MObject&      object,
    asr::ShaderGroup&   shaderGroup)
{
    return new RemapHsvExporter(object, shaderGroup);
}

RemapHsvExporter::RemapHsvExporter(
    const MObject&      object,
    asr::ShaderGroup&   shaderGroup)
  : ShadingNodeExporter(object, shaderGroup)
{
}

void RemapHsvExporter::exportParameterValue(
    const MPlug&        plug,
    const OSLParamInfo& paramInfo,
    asr::ParamArray&    shaderParams) const
{
    MFnDependencyNode depNodeFn(node());
    MStatus status;

    if (paramInfo.paramName == "in_hue_Position")
    {
        MPlug plug = depNodeFn.findPlug("hue", /*wantNetworkedPlug=*/ false, &status);

        std::vector<RemapHsvEntry> remapHue;
        remapHue.reserve(plug.numElements());

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

            remapHue.push_back(RemapHsvEntry(p, v, in));
        }

        std::stringstream ssp;
        ssp << "float[] ";

        std::stringstream ssv;
        ssv << "float[] ";

        std::stringstream ssi;
        ssi << "int[] ";

        for (size_t i = 0, e = remapHue.size(); i < e; ++i)
        {
            ssp << remapHue[i].m_pos    << " ";
            ssv << remapHue[i].m_value  << " ";
            ssi << remapHue[i].m_interp << " ";
        }

        shaderParams.insert("in_hue_Position"  , ssp.str().c_str());
        shaderParams.insert("in_hue_FloatValue", ssv.str().c_str());
        shaderParams.insert("in_hue_Interp"    , ssi.str().c_str());
    }
    else if (paramInfo.paramName == "in_saturation_Position")
    {
        MPlug plug = depNodeFn.findPlug("saturation", /*wantNetworkedPlug=*/ false, &status);

        std::vector<RemapHsvEntry> remapSaturation;
        remapSaturation.reserve(plug.numElements());

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

            remapSaturation.push_back(RemapHsvEntry(p, v, in));
        }

        std::stringstream ssp;
        ssp << "float[] ";

        std::stringstream ssv;
        ssv << "float[] ";

        std::stringstream ssi;
        ssi << "int[] ";

        for (size_t i = 0, e = remapSaturation.size(); i < e; ++i)
        {
            ssp << remapSaturation[i].m_pos    << " ";
            ssv << remapSaturation[i].m_value  << " ";
            ssi << remapSaturation[i].m_interp << " ";
        }

        shaderParams.insert("in_saturation_Position"  , ssp.str().c_str());
        shaderParams.insert("in_saturation_FloatValue", ssv.str().c_str());
        shaderParams.insert("in_saturation_Interp"    , ssi.str().c_str());
    }
    else if (paramInfo.paramName == "in_value_Position")
    {
        MPlug plug = depNodeFn.findPlug("value", /*wantNetworkedPlug=*/ false, &status);

        std::vector<RemapHsvEntry> remapValue;
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

            remapValue.push_back(RemapHsvEntry(p, v, in));
        }

        std::stringstream ssp;
        ssp << "float[] ";

        std::stringstream ssv;
        ssv << "float[] ";

        std::stringstream ssi;
        ssi << "int[] ";

        for (size_t i = 0, e = remapValue.size(); i < e; ++i)
        {
            ssp << remapValue[i].m_pos    << " ";
            ssv << remapValue[i].m_value  << " ";
            ssi << remapValue[i].m_interp << " ";
        }

        shaderParams.insert("in_value_Position"  , ssp.str().c_str());
        shaderParams.insert("in_value_FloatValue", ssv.str().c_str());
        shaderParams.insert("in_value_Interp"    , ssi.str().c_str());
    }
    else if (
        paramInfo.paramName == "in_hue_FloatValue" ||
        paramInfo.paramName == "in_hue_Interp" ||
        paramInfo.paramName == "in_saturation_FloatValue" ||
        paramInfo.paramName == "in_saturation_Interp" ||
        paramInfo.paramName == "in_value_FloatValue" ||
        paramInfo.paramName == "in_value_Interp"
        )
    {
        ; // Saved with resp. position attributes
    }
    else
    {
        ShadingNodeExporter::exportParameterValue(
            plug,
            paramInfo,
            shaderParams);
    }
}
