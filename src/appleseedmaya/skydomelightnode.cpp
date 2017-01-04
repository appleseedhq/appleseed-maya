
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
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnUnitAttribute.h>

// appleseed.maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/config.h"
#include "appleseedmaya/typeids.h"

const MString SkyDomeLightNode::nodeName("appleseedSkyDomeLight");
const MTypeId SkyDomeLightNode::id(SkyDomeLightNodeTypeId);
const MString SkyDomeLightNode::drawDbClassification("drawdb/geometry/appleseedSkyDomeLight");
const MString SkyDomeLightNode::drawRegistrantId("appleseedSkyDomeLight");

void* SkyDomeLightNode::creator()
{
    return new SkyDomeLightNode();
}

MStatus SkyDomeLightNode::initialize()
{
    EnvLightNode::initialize();

    MFnNumericAttribute numAttrFn;
    MFnMessageAttribute msgAttrFn;

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

    return status;
}

SkyDomeLightData::SkyDomeLightData()
  : EnvLightData()
{
}

MHWRender::MPxDrawOverride *SkyDomeLightDrawOverride::creator(const MObject& obj)
{
    return new SkyDomeLightDrawOverride(obj);
}

SkyDomeLightDrawOverride::SkyDomeLightDrawOverride(const MObject& obj)
  : EnvLightDrawOverride(obj)
{
}

MUserData *SkyDomeLightDrawOverride::prepareForDraw(
    const MDagPath&                 objPath,
    const MDagPath&                 cameraPath,
    const MHWRender::MFrameContext& frameContext,
    MUserData*                      oldData)
{
    // Retrieve data cache (create if does not exist)
    SkyDomeLightData *data =dynamic_cast<SkyDomeLightData*>(oldData);

    if (!data)
        data = new SkyDomeLightData();

    initializeData(objPath, *data);
    return data;
}
