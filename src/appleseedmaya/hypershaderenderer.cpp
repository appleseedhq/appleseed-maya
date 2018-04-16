
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
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

// Interface header.
#include "hypershaderenderer.h"

const MString HypershadeRenderer::name("appleseed");

void* HypershadeRenderer::creator()
{
    return new HypershadeRenderer();
}

HypershadeRenderer::HypershadeRenderer()
{
}

bool HypershadeRenderer::isSafeToUnload()
{
    return true;
}

MStatus HypershadeRenderer::startAsync(const JobParams& params)
{
    return MS::kSuccess;
}

MStatus HypershadeRenderer::stopAsync()
{
    return MS::kSuccess;
}

bool HypershadeRenderer::isRunningAsync()
{
    return true;
}

MStatus HypershadeRenderer::beginSceneUpdate()
{
    return MS::kSuccess;
}

MStatus HypershadeRenderer::endSceneUpdate()
{
    return MS::kSuccess;
}

MStatus HypershadeRenderer::destroyScene()
{
    return MS::kSuccess;
}

MStatus HypershadeRenderer::setProperty(const MUuid& id, const MString& name, bool value)
{
    return MS::kSuccess;
}

MStatus HypershadeRenderer::setProperty(const MUuid& id, const MString& name, int value)
{
    return MS::kSuccess;
}

MStatus HypershadeRenderer::setProperty(const MUuid& id, const MString& name, float value)
{
    return MS::kSuccess;
}

MStatus HypershadeRenderer::setProperty(const MUuid& id, const MString& name, const MString& value)
{
    return MS::kSuccess;
}

MStatus HypershadeRenderer::setShader(const MUuid& id, const MUuid& shaderId)
{
    return MS::kSuccess;
}

MStatus HypershadeRenderer::setResolution(unsigned int w, unsigned int h)
{
    return MS::kSuccess;
}


MStatus HypershadeRenderer::translateMesh(const MUuid& id, const MObject& node)
{
    return MS::kSuccess;
}

MStatus HypershadeRenderer::translateLightSource(const MUuid& id, const MObject& node)
{
    return MS::kSuccess;
}

MStatus HypershadeRenderer::translateCamera(const MUuid& id, const MObject& node)
{
    return MS::kSuccess;
}

MStatus HypershadeRenderer::translateEnvironment(const MUuid& id, EnvironmentType type)
{
    return MS::kSuccess;
}

MStatus HypershadeRenderer::translateTransform(const MUuid& id, const MUuid& childId, const MMatrix& matrix)
{
    return MS::kSuccess;
}

MStatus HypershadeRenderer::translateShader(const MUuid& id, const MObject& node)
{
    return MS::kSuccess;
}
