
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

#pragma once

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MPxRenderer.h>
#include "appleseedmaya/_endmayaheaders.h"

class HypershadeRenderer
  : public MPxRenderer
{
  public:
    static const MString name;

    static void* creator();

    HypershadeRenderer();

    bool isSafeToUnload() override;

    MStatus startAsync(const JobParams& params) override;
    MStatus stopAsync() override;
    bool isRunningAsync() override;

    MStatus beginSceneUpdate() override;
    MStatus endSceneUpdate() override;
    MStatus destroyScene() override;

    MStatus setProperty(const MUuid& id, const MString& name, bool value) override;
    MStatus setProperty(const MUuid& id, const MString& name, int value) override;
    MStatus setProperty(const MUuid& id, const MString& name, float value) override;
    MStatus setProperty(const MUuid& id, const MString& name, const MString& value) override;

    MStatus setShader(const MUuid& id, const MUuid& shaderId) override;
    MStatus setResolution(unsigned int width, unsigned int height) override;

    MStatus translateMesh(const MUuid& id, const MObject& node) override;
    MStatus translateLightSource(const MUuid& id, const MObject& node) override;
    MStatus translateCamera(const MUuid& id, const MObject& node) override;
    MStatus translateEnvironment(const MUuid& id, EnvironmentType type) override;
    MStatus translateTransform(const MUuid& id, const MUuid& childId, const MMatrix& matrix) override;
    MStatus translateShader(const MUuid& id, const MObject& node) override;
};

