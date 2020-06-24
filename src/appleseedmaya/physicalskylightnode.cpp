
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

// Interface header.
#include "physicalskylightnode.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/config.h"
#include "appleseedmaya/envlightdraw.h"
#include "appleseedmaya/typeids.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include "appleseedmaya/_endmayaheaders.h"

#include "renderer/utility/solarpositionalgorithm.h"

#include <cstring>

const MString PhysicalSkyLightNode::nodeName("appleseedPhysicalSkyLight");
const MTypeId PhysicalSkyLightNode::id(PhysicalSkyLightNodeTypeId);
const MString PhysicalSkyLightNode::drawDbClassification("drawdb/geometry/appleseedPhysicalSkyLight");

MObject PhysicalSkyLightNode::m_sunPositioningSystem;
MObject PhysicalSkyLightNode::m_sunTheta;
MObject PhysicalSkyLightNode::m_sunPhi;
MObject PhysicalSkyLightNode::m_hour;
MObject PhysicalSkyLightNode::m_minute;
MObject PhysicalSkyLightNode::m_second;
MObject PhysicalSkyLightNode::m_month;
MObject PhysicalSkyLightNode::m_day;
MObject PhysicalSkyLightNode::m_year;
MObject PhysicalSkyLightNode::m_timezone;
MObject PhysicalSkyLightNode::m_north;
MObject PhysicalSkyLightNode::m_latitude;
MObject PhysicalSkyLightNode::m_longitude;
MObject PhysicalSkyLightNode::m_turbidity;
MObject PhysicalSkyLightNode::m_turbidityMultiplier;
MObject PhysicalSkyLightNode::m_luminanceMultiplier;
MObject PhysicalSkyLightNode::m_luminanceGamma;
MObject PhysicalSkyLightNode::m_saturationMultiplier;
MObject PhysicalSkyLightNode::m_horizonShift;
MObject PhysicalSkyLightNode::m_groundAlbedo;
MObject PhysicalSkyLightNode::m_sunEnable;
MObject PhysicalSkyLightNode::m_sunRadianceMultiplier;
MObject PhysicalSkyLightNode::m_sunSizeMultiplier;
MObject PhysicalSkyLightNode::m_message;
MObject PhysicalSkyLightNode::m_displaySize;

void* PhysicalSkyLightNode::creator()
{
    return new PhysicalSkyLightNode();
}

MStatus PhysicalSkyLightNode::initialize()
{
    MFnEnumAttribute enumAttrFn;
    MFnMessageAttribute msgAttrFn;
    MFnNumericAttribute numAttrFn;
    MFnUnitAttribute unitAttrFn;

    MStatus status;

    // Render globals connection.
    m_message = msgAttrFn.create("globalsMessage", "globalsMessage", &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");

    status = addAttribute(m_message);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    // Display size.
    m_displaySize = numAttrFn.create("size", "sz", MFnNumericData::kFloat, 1.0f, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");

    numAttrFn.setMin(0.01);
    numAttrFn.setMax(100.0);
    status = addAttribute(m_displaySize);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_turbidity = numAttrFn.create(
        "turbidity",
        "turbidity",
        MFnNumericData::kFloat,
        1.0,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    numAttrFn.setNiceNameOverride("Turbidity");
    status = addAttribute(m_turbidity);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_turbidityMultiplier = numAttrFn.create(
        "turbidityScale",
        "turbidityScale",
        MFnNumericData::kFloat,
        2.0,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    numAttrFn.setMin(0.0);
    numAttrFn.setMax(8.0);
    numAttrFn.setNiceNameOverride("Turbidity Scale");
    status = addAttribute(m_turbidityMultiplier);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_luminanceMultiplier = numAttrFn.create(
        "luminanceScale",
        "luminanceScale",
        MFnNumericData::kFloat,
        1.0,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    numAttrFn.setMin(0.0);
    numAttrFn.setMax(10.0);
    numAttrFn.setNiceNameOverride("Luminance Scale");
    status = addAttribute(m_luminanceMultiplier);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_luminanceGamma = numAttrFn.create(
        "luminanceGamma",
        "luminanceGamma",
        MFnNumericData::kFloat,
        1.0,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    numAttrFn.setMin(0.0);
    numAttrFn.setMax(3.0);
    numAttrFn.setNiceNameOverride("Luminance Gamma");
    status = addAttribute(m_luminanceGamma);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_saturationMultiplier = numAttrFn.create(
        "saturationScale",
        "saturationScale",
        MFnNumericData::kFloat,
        1.0,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    numAttrFn.setMin(0.0);
    numAttrFn.setMax(10.0);
    numAttrFn.setNiceNameOverride("Saturation Scale");
    status = addAttribute(m_saturationMultiplier);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_horizonShift = unitAttrFn.create(
        "horizonShift",
        "horizonShift",
        MFnUnitAttribute::kAngle,
        0.0,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    unitAttrFn.setNiceNameOverride("Horizon Shift");
    unitAttrFn.setMin(-360.0);
    unitAttrFn.setMax(360.0);
    status = addAttribute(m_horizonShift);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_groundAlbedo = numAttrFn.create(
        "groundAlbedo",
        "groundAlbedo",
        MFnNumericData::kFloat,
        0.3,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    numAttrFn.setMin(0.0);
    numAttrFn.setMax(1.0);
    numAttrFn.setNiceNameOverride("Ground Albedo");
    status = addAttribute(m_groundAlbedo);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");
    
    m_sunPositioningSystem = enumAttrFn.create(
        "sunPositioningSystem",
        "sunPositioningSystem",
        SunPositioningSystem::Analytical,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    enumAttrFn.addField("Sun Theta/Sun Phi", SunPositioningSystem::Analytical);
    enumAttrFn.addField("Time and Location", SunPositioningSystem::TimeLocation);
    status = addAttribute(m_sunPositioningSystem);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_sunTheta = unitAttrFn.create(
        "sunTheta",
        "sunTheta",
        MFnUnitAttribute::kAngle,
        M_PI * 0.25, // 45 degrees
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    unitAttrFn.setNiceNameOverride("Sun Theta Angle");
    unitAttrFn.setMin(0.0);
    unitAttrFn.setMax(M_PI * 0.5);
    status = addAttribute(m_sunTheta);

    m_sunPhi = unitAttrFn.create(
        "sunPhi",
        "sunPhi",
        MFnUnitAttribute::kAngle,
        0.0,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    unitAttrFn.setNiceNameOverride("Sun Phi Angle");
    unitAttrFn.setMin(-M_PI * 2.0);
    unitAttrFn.setMax(M_PI * 2.0);
    status = addAttribute(m_sunPhi);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_hour = numAttrFn.create(
        "hour",
        "hour",
        MFnNumericData::kInt,
        12,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    numAttrFn.setNiceNameOverride("Hour");
    numAttrFn.setMin(0);
    numAttrFn.setMax(23);
    status = addAttribute(m_hour);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_minute = numAttrFn.create(
        "minute",
        "minute",
        MFnNumericData::kInt,
        0,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    numAttrFn.setNiceNameOverride("Minute");
    numAttrFn.setMin(0);
    numAttrFn.setMax(59);
    status = addAttribute(m_minute);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_second = numAttrFn.create(
        "second",
        "second",
        MFnNumericData::kInt,
        0,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    numAttrFn.setNiceNameOverride("Second");
    numAttrFn.setMin(0);
    numAttrFn.setMax(59);
    status = addAttribute(m_second);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_month = numAttrFn.create(
        "month",
        "month",
        MFnNumericData::kInt,
        1,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    numAttrFn.setNiceNameOverride("Month");
    numAttrFn.setMin(1);
    numAttrFn.setMax(12);
    status = addAttribute(m_month);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_day = numAttrFn.create(
        "day",
        "day",
        MFnNumericData::kInt,
        1,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    numAttrFn.setNiceNameOverride("Day");
    numAttrFn.setMin(1);
    numAttrFn.setMax(31);
    status = addAttribute(m_day);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_year = numAttrFn.create(
        "year",
        "year",
        MFnNumericData::kInt,
        2020,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    numAttrFn.setNiceNameOverride("Year");
    numAttrFn.setMin(-2000);
    numAttrFn.setMax(4000);
    status = addAttribute(m_year);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_timezone = numAttrFn.create(
        "timezone",
        "timezone",
        MFnNumericData::kInt,
        0,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    numAttrFn.setNiceNameOverride("Timezone");
    numAttrFn.setMin(-18);
    numAttrFn.setMax(18);
    status = addAttribute(m_timezone);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_north = numAttrFn.create(
        "north",
        "north",
        MFnNumericData::kFloat,
        0.0,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    numAttrFn.setNiceNameOverride("North");
    numAttrFn.setMin(-180.0);
    numAttrFn.setMax(180.0);
    status = addAttribute(m_north);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_latitude = numAttrFn.create(
        "latitude",
        "latitude",
        MFnNumericData::kFloat,
        0.0,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    numAttrFn.setNiceNameOverride("Latitude");
    numAttrFn.setMin(-90.0);
    numAttrFn.setMax(90.0);
    status = addAttribute(m_latitude);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_longitude = numAttrFn.create(
        "longitude",
        "longitude",
        MFnNumericData::kFloat,
        0.0,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    numAttrFn.setNiceNameOverride("Longitude");
    numAttrFn.setMin(-90.0);
    numAttrFn.setMax(90.0);
    status = addAttribute(m_longitude);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_sunEnable = numAttrFn.create(
        "sunEnable",
        "sunEnable",
        MFnNumericData::kBoolean,
        true,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    numAttrFn.setNiceNameOverride("Sun Light");
    status = addAttribute(m_sunEnable);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_sunRadianceMultiplier = numAttrFn.create(
        "sunRadianceScale",
        "sunRadianceScale",
        MFnNumericData::kFloat,
        1.0,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    numAttrFn.setMin(0.0);
    numAttrFn.setMax(10.0);
    numAttrFn.setNiceNameOverride("Sun Intensity Scale");
    status = addAttribute(m_sunRadianceMultiplier);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    m_sunSizeMultiplier = numAttrFn.create(
        "sunSizeScale",
        "sunSizeScale",
        MFnNumericData::kFloat,
        1.0,
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create physical sky light attribute");
    numAttrFn.setMin(0.0);
    numAttrFn.setMax(100.0);
    numAttrFn.setNiceNameOverride("Sun Size Scale");
    status = addAttribute(m_sunSizeMultiplier);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add physical sky light attribute");

    attributeAffects(m_sunPositioningSystem, m_sunTheta);
    attributeAffects(m_hour, m_sunTheta);
    attributeAffects(m_minute, m_sunTheta);
    attributeAffects(m_second, m_sunTheta);
    attributeAffects(m_year, m_sunTheta);
    attributeAffects(m_day, m_sunTheta);
    attributeAffects(m_month, m_sunTheta);
    attributeAffects(m_timezone, m_sunTheta);
    attributeAffects(m_north, m_sunTheta);
    attributeAffects(m_latitude, m_sunTheta);
    attributeAffects(m_longitude, m_sunTheta);

    attributeAffects(m_sunPositioningSystem, m_sunPhi);
    attributeAffects(m_hour, m_sunPhi);
    attributeAffects(m_minute, m_sunPhi);
    attributeAffects(m_second, m_sunPhi);
    attributeAffects(m_year, m_sunPhi);
    attributeAffects(m_day, m_sunPhi);
    attributeAffects(m_month, m_sunPhi);
    attributeAffects(m_timezone, m_sunPhi);
    attributeAffects(m_north, m_sunPhi);
    attributeAffects(m_latitude, m_sunPhi);
    attributeAffects(m_longitude, m_sunPhi);

    return status;
}

bool PhysicalSkyLightNode::isBounded() const
{
    return true;
}

MBoundingBox PhysicalSkyLightNode::boundingBox() const
{
    float size = 1.0f;
    AttributeUtils::get(thisMObject(), "size", size);
    return sphereAndLogoBoundingBox(size);
}

void PhysicalSkyLightNode::draw(
    M3dView&                view,
    const MDagPath&         path,
    M3dView::DisplayStyle   style,
    M3dView::DisplayStatus  status)
{
    float size = 1.0f;
    AttributeUtils::get(thisMObject(), "size", size);

    view.beginGL();
    glPushAttrib(GL_CURRENT_BIT);

    switch (status)
    {
      case M3dView::kActive:
        view.setDrawColor(18, M3dView::kActiveColors);
      break;

      case M3dView::kActiveAffected:
        view.setDrawColor(19, M3dView::kActiveColors);
      break;

      case M3dView::kLead:
        view.setDrawColor(22, M3dView::kActiveColors);
      break;

      default:
        view.setDrawColor(3, M3dView::kActiveColors);
      break;
    }

    if (style == M3dView::kFlatShaded || style == M3dView::kGouraudShaded)
        drawSphereWireframe(size);
    else
        drawSphereWireframe(size);

    drawAppleseedLogo(size);

    glPopAttrib();
    view.endGL();
}

MStatus PhysicalSkyLightNode::compute(const MPlug& plug, MDataBlock& dataBlock)
{
    static MAngle sunPhi = 0.0f;

    MGlobal::displayInfo(plug.info());

    if (dataBlock.inputValue(m_sunPositioningSystem).asInt() == 1)
    {
        if (strstr(plug.info().asChar(), "sunTheta") != nullptr)
        {

            renderer::ParamArray param;
            param.insert("hour", dataBlock.inputValue(m_hour).asInt());
            param.insert("minute", dataBlock.inputValue(m_minute).asInt());
            param.insert("second", dataBlock.inputValue(m_second).asInt());
            param.insert("month", dataBlock.inputValue(m_month).asInt());
            param.insert("day", dataBlock.inputValue(m_day).asInt());
            param.insert("year", dataBlock.inputValue(m_year).asInt());
            param.insert("timezone", dataBlock.inputValue(m_timezone).asInt());
            param.insert("north", dataBlock.inputValue(m_north).asInt());
            param.insert("latitude", dataBlock.inputValue(m_latitude).asInt());
            param.insert("longitude", dataBlock.inputValue(m_longitude).asInt());

            foundation::auto_release_ptr<renderer::SunPositioner> sunPositioner =
                renderer::SunPositionerFactory::create("Sun Positioner", param);

            sunPositioner->fetch_data();
            sunPositioner->compute_sun_position();

            dataBlock.outputValue(plug).asDouble() = MAngle(sunPositioner->get_zenith(), MAngle::Unit::kDegrees).asRadians();
            sunPhi = MAngle(sunPositioner->get_azimuth(), MAngle::Unit::kDegrees);
        }
        else 
            if ((strstr(plug.info().asChar(), "sunPhi") != nullptr))
                dataBlock.outputValue(plug).asDouble() = sunPhi.asRadians();
    }
    else 
        return MS::kUnknownParameter;

    return MS::kSuccess;
}

PhysicalSkyLightData::PhysicalSkyLightData()
  : MUserData(false) // don't delete after draw
{
}

MHWRender::MPxDrawOverride* PhysicalSkyLightDrawOverride::creator(const MObject& obj)
{
    return new PhysicalSkyLightDrawOverride(obj);
}

PhysicalSkyLightDrawOverride::PhysicalSkyLightDrawOverride(const MObject& obj)
  : MHWRender::MPxDrawOverride(obj, PhysicalSkyLightDrawOverride::draw)
{
}

MHWRender::DrawAPI PhysicalSkyLightDrawOverride::supportedDrawAPIs() const
{
    return MHWRender::kOpenGL | MHWRender::kOpenGLCoreProfile;
}

bool PhysicalSkyLightDrawOverride::isBounded(const MDagPath& objPath, const MDagPath& cameraPath) const
{
    return true;
}

MBoundingBox PhysicalSkyLightDrawOverride::boundingBox(
    const MDagPath&                 objPath,
    const MDagPath&                 cameraPath) const
{
    float size = 1.0f;
    AttributeUtils::get(objPath.node(), "size", size);
    return sphereAndLogoBoundingBox(size);
}

MUserData* PhysicalSkyLightDrawOverride::prepareForDraw(
    const MDagPath&                 objPath,
    const MDagPath&                 cameraPath,
    const MHWRender::MFrameContext& frameContext,
    MUserData*                      oldData)
{
    // Retrieve data cache (create if does not exist)
    PhysicalSkyLightData* data = dynamic_cast<PhysicalSkyLightData*>(oldData);

    if (!data)
        data = new PhysicalSkyLightData();

    data->m_size = 1.0f;
    AttributeUtils::get(objPath.node(), "size", data->m_size);
    data->m_color = MHWRender::MGeometryUtilities::wireframeColor(objPath);
    return data;
}

void PhysicalSkyLightDrawOverride::draw(
    const MHWRender::MDrawContext&  context,
    const MUserData*                data)
{
    const PhysicalSkyLightData* drawData = dynamic_cast<const PhysicalSkyLightData*>(data);
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

    MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer();
    if (!renderer)
        return;

    if (renderer->drawAPIIsOpenGL())
    {
        float color [3] = { drawData->m_color.r, drawData->m_color.g, drawData->m_color.b};
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
