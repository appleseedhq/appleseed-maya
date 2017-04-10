
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016-2017 Esteban Tovagliari, The appleseedhq Organization
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

// Standard headers.
#include <cmath>

// Maya headers.
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnNumericAttribute.h>

// appleseed.foundation headers.
#include "foundation/math/scalar.h"
#include "foundation/math/vector.h"

// appleseed.maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/config.h"
#include "appleseedmaya/envlightdraw.h"

namespace asf = foundation;

MObject EnvLightNode::m_message;
MObject EnvLightNode::m_displaySize;

float EnvLightNode::displaySize() const
{
    float size = 1.0f;
    AttributeUtils::get(thisMObject(), "size", size);
    return size;
}

MStatus EnvLightNode::compute(const MPlug& plug, MDataBlock& dataBlock)
{
    return MS::kUnknownParameter;
}

void EnvLightNode::draw(
    M3dView&                view,
    const MDagPath&         path,
    M3dView::DisplayStyle   style,
    M3dView::DisplayStatus  status)
{
    const float size = displaySize();

    view.beginGL();
    glPushAttrib(GL_CURRENT_BIT);

    if (status == M3dView::kActive)
        view.setDrawColor(13, M3dView::kActiveColors);
    else
        view.setDrawColor(13, M3dView::kDormantColors);

    if (style == M3dView::kFlatShaded || style == M3dView::kGouraudShaded)
        drawSphereWireframe(size);
    else
        drawSphereWireframe(size);

    drawAppleseedLogo(size);

    glPopAttrib();
    view.endGL();
}

bool EnvLightNode::isBounded() const
{
    return true;
}

MBoundingBox EnvLightNode::boundingBox() const
{
    const float size = displaySize();
    return sphereAndLogoBoundingBox(size);
}

EnvLightData::EnvLightData()
  : MUserData(false) // don't delete after draw
{
}

EnvLightDrawOverride::EnvLightDrawOverride(const MObject& obj)
  : MHWRender::MPxDrawOverride(obj, EnvLightDrawOverride::draw)
{
}

MHWRender::DrawAPI EnvLightDrawOverride::supportedDrawAPIs() const
{
    return MHWRender::kOpenGL | MHWRender::kOpenGLCoreProfile;
}

MBoundingBox EnvLightDrawOverride::boundingBox(const MDagPath& objPath, const MDagPath& cameraPath) const
{
    float size = 1.0f;
    AttributeUtils::get(objPath.node(), "size", size);
    return sphereAndLogoBoundingBox(size);
}

void EnvLightDrawOverride::draw(const MHWRender::MDrawContext& context, const MUserData *data)
{
    const EnvLightData *drawData = dynamic_cast<const EnvLightData*>(data);
    if (!drawData)
        return;

    MStatus status;

    const MMatrix transform = context.getMatrix(MHWRender::MDrawContext::kWorldViewMtx, &status);
    if (status != MStatus::kSuccess)
        return;

    const MMatrix projection = context.getMatrix(MHWRender::MDrawContext::kProjectionMtx, &status);
    if (status != MStatus::kSuccess)
        return;

    const unsigned int displayStyle = context.getDisplayStyle();

    MHWRender::MRenderer *renderer = MHWRender::MRenderer::theRenderer();
    if (!renderer)
        return;

    if (renderer->drawAPIIsOpenGL())
    {
        float color [3] ={ drawData->m_color.r, drawData->m_color.g, drawData->m_color.b};
        glColor3fv(color);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixd(transform.matrix[0]);

        glMatrixMode (GL_PROJECTION);
        glPushMatrix();
        glLoadMatrixd(projection.matrix[0]);

        glPushAttrib(GL_CURRENT_BIT);

        if (displayStyle & MHWRender::MDrawContext::kGouraudShaded)
            drawSphereWireframe(drawData->m_size);

        if (displayStyle & MHWRender::MDrawContext::kWireFrame)
            drawSphereWireframe(drawData->m_size);

        drawAppleseedLogo(drawData->m_size);

        glPopAttrib();

        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }
}

void EnvLightDrawOverride::initializeData(const MDagPath &objPath, EnvLightData &data) const
{
    data.m_size = 1.0f;
    AttributeUtils::get(objPath.node(), "size", data.m_size);

    data.m_color = MHWRender::MGeometryUtilities::wireframeColor(objPath);
}
