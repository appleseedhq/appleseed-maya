
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

// Interface header.
#include "appleseedmaya/exporters/dagnodeexporter.h"


namespace asf = foundation;
namespace asr = renderer;

DagNodeExporter::DagNodeExporter(
    const MDagPath&                 path,
    asr::Project&                   project,
    AppleseedSession::SessionMode   sessionMode)
  : MPxNodeExporter(path.node(), project, sessionMode)
  , m_path(path)
{
}

bool DagNodeExporter::supportsMotionBlur() const
{
    return true;
}

void DagNodeExporter::collectMotionBlurSteps(MotionBlurTimes& motionTimes) const
{
}

void DagNodeExporter::exportCameraMotionStep(float time)
{
}

void DagNodeExporter::exportTransformMotionStep(float time)
{
}

void DagNodeExporter::exportShapeMotionStep(float time)
{
}

MString DagNodeExporter::appleseedName() const
{
    return dagPath().fullPathName();
}

const MDagPath& DagNodeExporter::dagPath() const
{
    return m_path;
}

asf::Matrix4d DagNodeExporter::convert(const MMatrix& m) const
{
    asf::Matrix4d result;

    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
            result(i, j) = m[j][i];
    }

    return result;
}
