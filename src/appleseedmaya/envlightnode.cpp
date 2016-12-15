
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
#include "appleseedmaya/envlightnode.h"

// Maya headers.
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnNumericAttribute.h>

// appleseed.maya headers.
#include "appleseedmaya/config.h"

MObject EnvLightNode::m_message;
MObject EnvLightNode::m_displaySize;

MStatus EnvLightNode::compute(const MPlug& plug, MDataBlock& dataBlock)
{
    return MS::kUnknownParameter;
}

void EnvLightNode::draw(
    M3dView&                view,
    const MDagPath &        path,
    M3dView::DisplayStyle   style,
    M3dView::DisplayStatus  status)
{
}

bool EnvLightNode::isBounded() const
{
    return true;
}

MBoundingBox EnvLightNode::boundingBox() const
{
    return MBoundingBox();
}

EnvLightData::EnvLightData()
  : MUserData (false) // don't delete after draw
{
}

MHWRender::MPxDrawOverride *EnvLightDrawOverride::creator(const MObject& obj)
{
    return new EnvLightDrawOverride(obj);
}

EnvLightDrawOverride::EnvLightDrawOverride(const MObject& obj)
  : MHWRender::MPxDrawOverride(obj, EnvLightDrawOverride::draw)
{
}

MHWRender::DrawAPI EnvLightDrawOverride::supportedDrawAPIs() const
{
    return MHWRender::kOpenGL;
}

MBoundingBox EnvLightDrawOverride::boundingBox(const MDagPath& objPath, const MDagPath& cameraPath) const
{
    float size = 1.0f;
    MBoundingBox boundingBox(
        MPoint(-size, -size, -size),
        MPoint( size,  size,  size));

    return boundingBox;
}

MUserData *EnvLightDrawOverride::prepareForDraw(
    const MDagPath&                 objPath,
    const MDagPath&                 cameraPath,
    const MHWRender::MFrameContext& frameContext,
    MUserData*                      oldData)
{
    // Retrieve data cache (create if does not exist)
    EnvLightData *data =dynamic_cast<EnvLightData*>(oldData);

    if(!data)
        data = new EnvLightData();

    return data;
}

void EnvLightDrawOverride::draw(const MHWRender::MDrawContext& context, const MUserData *data)
{
    // todo: implement this...
}
