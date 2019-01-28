
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

#ifndef APPLESEED_MAYA_SWATCHRENDERER_H
#define APPLESEED_MAYA_SWATCHRENDERER_H

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MSwatchRenderBase.h>
#include "appleseedmaya/_endmayaheaders.h"

class SwatchRenderer
  : public MSwatchRenderBase
{
  public:
    static const MString name;
    static const MString fullName;

    static void initialize(const MString& pluginPath);
    static void uninitialize();

    static MSwatchRenderBase* creator(
        MObject dependNode,
        MObject renderNode,
        int     imageResolution);

    bool doIteration() override;

  private:
    SwatchRenderer(
        MObject dependNode,
        MObject renderNode,
        int     imageResolution);
};

#endif  // !APPLESEED_MAYA_SWATCHRENDERER_H
