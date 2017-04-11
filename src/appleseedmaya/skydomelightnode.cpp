
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
#include "appleseedmaya/skydomelightnode.h"

// Maya headers.
#include <maya/MFileObject.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>

// appleseed.maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/config.h"
#include "appleseedmaya/envlightdraw.h"
#include "appleseedmaya/typeids.h"

const MString SkyDomeLightNode::nodeName("appleseedSkyDomeLight");
const MTypeId SkyDomeLightNode::id(SkyDomeLightNodeTypeId);
const MString SkyDomeLightNode::drawDbClassification("drawdb/geometry/appleseedSkyDomeLight");

MObject SkyDomeLightNode::m_map;
MObject SkyDomeLightNode::m_intensity;
MObject SkyDomeLightNode::m_exposure;
MObject SkyDomeLightNode::m_horizontalShift;
MObject SkyDomeLightNode::m_verticalShift;
MObject SkyDomeLightNode::m_message;
MObject SkyDomeLightNode::m_displaySize;

void* SkyDomeLightNode::creator()
{
    return new SkyDomeLightNode();
}

MStatus SkyDomeLightNode::initialize()
{
    MFnNumericAttribute numAttrFn;
    MFnMessageAttribute msgAttrFn;
    MFnTypedAttribute typedAttrFn;
    MFnUnitAttribute unitAttrFn;

    MStatus status;

    // Render globals connection.
    m_message = msgAttrFn.create("globalsMessage", "globalsMessage", &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create envLight attribute");

    status = addAttribute(m_message);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add envLight attribute");

    // Display size.
    m_displaySize = numAttrFn.create("size", "sz", MFnNumericData::kFloat, 1.0f, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create envLight attribute");

    numAttrFn.setMin(0.01f);
    numAttrFn.setMax(100.0f);
    status = addAttribute(m_displaySize);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add envLight attribute");

    // Map.
    m_map = typedAttrFn.create("map", "map", MFnData::kString);
    typedAttrFn.setUsedAsFilename(true);
    status = addAttribute(m_map);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add envLight attribute");

    // Intensity.
    m_intensity = numAttrFn.create(
        "intensity",
        "intensity",
        MFnNumericData::kFloat,
        1.0,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create envLight attribute");
    numAttrFn.setMin(0.0);
    numAttrFn.setNiceNameOverride("Intensity");
    status = addAttribute(m_intensity);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add envLight attribute");

    // Exposure.
    m_exposure = numAttrFn.create(
        "exposure",
        "exposure",
        MFnNumericData::kFloat,
        1.0,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create envLight attribute");
    numAttrFn.setMin(0.0);
    numAttrFn.setNiceNameOverride("Exposure");
    status = addAttribute(m_exposure);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add envLight attribute");

    // Horizontal shift.
    m_horizontalShift = unitAttrFn.create(
        "hShift",
        "hShift",
        MFnUnitAttribute::kAngle,
        0.0,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create sky dome light attribute");
    unitAttrFn.setNiceNameOverride("Horizontal Shift");
    unitAttrFn.setMin(-M_PI * 2.0);
    unitAttrFn.setMax(M_PI * 2.0);
    status = addAttribute(m_horizontalShift);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add sky dome light attribute");

    // Vertical shift.
    m_verticalShift = unitAttrFn.create(
        "vShift",
        "vShift",
        MFnUnitAttribute::kAngle,
        0.0,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create sky dome light attribute");
    unitAttrFn.setNiceNameOverride("Vertical Shift");
    unitAttrFn.setMin(-M_PI * 2.0);
    unitAttrFn.setMax(M_PI * 2.0);
    status = addAttribute(m_verticalShift);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add sky dome light attribute");

    return status;
}

bool SkyDomeLightNode::isBounded() const
{
    return true;
}

MBoundingBox SkyDomeLightNode::boundingBox() const
{
    float size = 1.0f;
    AttributeUtils::get(thisMObject(), "size", size);
    return sphereAndLogoBoundingBox(size);
}

MStatus SkyDomeLightNode::compute(const MPlug& plug, MDataBlock& dataBlock)
{
    return MS::kUnknownParameter;
}

void SkyDomeLightNode::draw(
    M3dView&                view,
    const MDagPath&         path,
    M3dView::DisplayStyle   style,
    M3dView::DisplayStatus  status)
{
    float size = 1.0f;
    AttributeUtils::get(thisMObject(), "size", size);

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

SkyDomeLightData::SkyDomeLightData()
  : MUserData(false) // don't delete after draw
{
}

MStringArray SkyDomeLightNode::getFilesToArchive(
    bool                            shortName,
    bool                            unresolvedName,
    bool                            markCouldBeImageSequence) const
{
    MStringArray files;
    MStatus status = MS::kSuccess;

    MPlug fileNamePlug(thisMObject(), m_map);
    MString fileName = fileNamePlug.asString(MDGContext::fsNormal, &status);

    if (status == MS::kSuccess && fileName.length() > 0)
    {
        if(unresolvedName)
            files.append(fileName);
        else
        {
            //unresolvedName is false, resolve the path via MFileObject.
            MFileObject fileObject;
            fileObject.setRawFullName(fileName);
            files.append(fileObject.resolvedFullName());
        }
    }

    return files;
}

void SkyDomeLightNode::getExternalContent(MExternalContentInfoTable& table) const
{
   addExternalContentForFileAttr(table, m_map);
   MPxNode::getExternalContent(table);
}

void SkyDomeLightNode::setExternalContent(const MExternalContentLocationTable& table)
{
   setExternalContentForFileAttr(m_map, table);
   MPxNode::setExternalContent(table);
}

MHWRender::MPxDrawOverride *SkyDomeLightDrawOverride::creator(const MObject& obj)
{
    return new SkyDomeLightDrawOverride(obj);
}

SkyDomeLightDrawOverride::SkyDomeLightDrawOverride(const MObject& obj)
  : MHWRender::MPxDrawOverride(obj, SkyDomeLightDrawOverride::draw)
{
}

MHWRender::DrawAPI SkyDomeLightDrawOverride::supportedDrawAPIs() const
{
    return MHWRender::kOpenGL | MHWRender::kOpenGLCoreProfile;
}

MBoundingBox SkyDomeLightDrawOverride::boundingBox(
    const MDagPath&                 objPath,
    const MDagPath&                 cameraPath) const
{
    float size = 1.0f;
    AttributeUtils::get(objPath.node(), "size", size);
    return sphereAndLogoBoundingBox(size);
}

MUserData* SkyDomeLightDrawOverride::prepareForDraw(
    const MDagPath&                 objPath,
    const MDagPath&                 cameraPath,
    const MHWRender::MFrameContext& frameContext,
    MUserData*                      oldData)
{
    // Retrieve data cache (create if does not exist)
    SkyDomeLightData *data =dynamic_cast<SkyDomeLightData*>(oldData);

    if (!data)
        data = new SkyDomeLightData();

    data->m_size = 1.0f;
    AttributeUtils::get(objPath.node(), "size", data->m_size);
    data->m_color = MHWRender::MGeometryUtilities::wireframeColor(objPath);
    return data;
}

void SkyDomeLightDrawOverride::draw(
    const MHWRender::MDrawContext&  context,
    const MUserData*                data)
{
    const SkyDomeLightData *drawData = dynamic_cast<const SkyDomeLightData*>(data);
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
