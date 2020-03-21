
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2019 Esteban Tovagliari, The appleseedhq Organization
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

#pragma once

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MColor.h>
#include <maya/MColorArray.h>
#include <maya/MFloatArray.h>
#include "appleseedmaya/_endmayaheaders.h"

// Standard headers.
#include <sstream>
#include <string>
#include <vector>

template <typename T>
struct RampEntry
{
    RampEntry(int index, float pos, const T& value)
      : m_index(index)
      , m_pos(pos)
      , m_value(value)
    {
    }

    bool operator<(const RampEntry<T>& other) const
    {
        return m_pos < other.m_pos;
    }

    int     m_index;
    float   m_pos;
    T       m_value;
};

template <typename T> struct RampEntryTraits {};

template <> struct RampEntryTraits<MColor>
{
    typedef MColorArray ArrayType;

    static const char* paramValueTypeName()
    {
        return "color[]";
    }

    static void outputValue(std::stringstream& ss, const MColor& value)
    {
        ss << value.r << " " << value.g << " " << value.b << " ";
    }
};

template <> struct RampEntryTraits<float>
{
    typedef MFloatArray ArrayType;

    static const char* paramValueTypeName()
    {
        return "float[]";
    }

    static void outputValue(std::stringstream& ss, const float& value)
    {
        ss << value << " ";
    }
};

template <typename T>
void serializeRamp(
    const std::vector<RampEntry<T>>& entries,
    std::string&                     outValues,
    std::string&                     outPositions)
{
    std::stringstream ssp;
    ssp << "float[] ";

    std::stringstream ssv;
    ssv << RampEntryTraits<T>::paramValueTypeName() << " ";

    for (size_t i = 0, e = entries.size(); i < e; ++i)
    {
        ssp << entries[i].m_pos << " ";
        RampEntryTraits<T>::outputValue(ssv, entries[i].m_value);
    }

    outValues = ssv.str();
    outPositions = ssp.str();
}

