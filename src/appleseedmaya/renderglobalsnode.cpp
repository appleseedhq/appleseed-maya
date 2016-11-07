
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
#include "appleseedmaya/renderglobalsnode.h"

// appleseed.maya headers.
#include "appleseedmaya/config.h"
#include "appleseedmaya/typeids.h"

// appleseed.renderer headers.
#include "renderer/api/utility.h"

// Maya headers.
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnNumericAttribute.h>


const MString RenderGlobalsNode::nodeName("appleseedRenderGlobals");
const MTypeId RenderGlobalsNode::id(RenderGlobalsNodeTypeId);

MObject RenderGlobalsNode::m_pixelSamples;
MObject RenderGlobalsNode::m_passes;
MObject RenderGlobalsNode::m_tileSize;
MObject RenderGlobalsNode::m_gi;
MObject RenderGlobalsNode::m_caustics;
MObject RenderGlobalsNode::m_bounces;
MObject RenderGlobalsNode::m_backgroundEmitsLight;
MObject RenderGlobalsNode::m_renderingThreads;
MObject RenderGlobalsNode::m_envLightNode;

void* RenderGlobalsNode::creator()
{
    return new RenderGlobalsNode();
}

MStatus RenderGlobalsNode::initialize()
{
    MFnNumericAttribute numAttrFn;
	MFnMessageAttribute msgAttrFn;

    MStatus status;

    // Pixel Samples.
    m_pixelSamples = numAttrFn.create("samples", "samples", MFnNumericData::kInt, 16, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals samples attribute");

    numAttrFn.setMin(1);
    status = addAttribute(m_pixelSamples);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals samples attribute");

    // Render Passes.
    m_passes = numAttrFn.create("passes", "passes", MFnNumericData::kInt, 1, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals passes attribute");

    numAttrFn.setMin(1);
    status = addAttribute(m_passes);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals passes attribute");

    // Tile Size.
    m_tileSize = numAttrFn.create("tileSize", "tileSize", MFnNumericData::kInt, 64, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals tileSize attribute");

    numAttrFn.setMin(1);
    status = addAttribute(m_tileSize);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals tileSize attribute");

    // GI.
    m_gi = numAttrFn.create("gi", "gi", MFnNumericData::kBoolean, true, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals gi attribute");

    status = addAttribute(m_gi);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals gi attribute");

    // Caustics.
    m_caustics = numAttrFn.create("caustics", "caustics", MFnNumericData::kBoolean, false, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals caustics attribute");

    status = addAttribute(m_caustics);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals caustics attribute");

    // GI Bounces.
    m_bounces = numAttrFn.create("bounces", "bounces", MFnNumericData::kInt, 8, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals bounces attribute");

    numAttrFn.setMin(0);
    status = addAttribute(m_bounces);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals bounces attribute");

    // Background emits light.
    m_backgroundEmitsLight = numAttrFn.create("bgLight", "bgLight", MFnNumericData::kBoolean, true, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals bgLight attribute");

    status = addAttribute(m_backgroundEmitsLight);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals bgLight attribute");

    // Rendering threads.
    m_renderingThreads = numAttrFn.create("threads", "threads", MFnNumericData::kInt, 0, &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals threads attribute");

    status = addAttribute(m_renderingThreads);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals threads attribute");

    // Environment light connection.
	m_envLightNode = msgAttrFn.create("envLight", "env", &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to create render globals envLight attribute");

    status = addAttribute(m_envLightNode);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: Failed to add render globals envLight attribute");

    return status;
}

MStatus RenderGlobalsNode::compute(const MPlug& plug, MDataBlock& dataBlock)
{
    return MS::kSuccess;
}
