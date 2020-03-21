
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
#include <maya/MColor.h>
#include <maya/MDrawContext.h>
#include <maya/MDrawRegistry.h>
#include <maya/MHWGeometryUtilities.h>
#include <maya/MObject.h>
#include <maya/MPointArray.h>
#include <maya/MPxDrawOverride.h>
#include <maya/MPxLocatorNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MUserData.h>
#include <maya/MViewport2Renderer.h>
#include "appleseedmaya/_endmayaheaders.h"

class SkyDomeLightNode
  : public MPxLocatorNode
{
  public:
    static const MString nodeName;
    static const MTypeId id;
    static const MString drawDbClassification;

    static void* creator();
    static MStatus initialize();

    MStatus compute(const MPlug& plug, MDataBlock& data) override;

    bool isBounded() const override;
    MBoundingBox boundingBox() const override;

    void draw(
        M3dView&                view,
        const MDagPath&         path,
        M3dView::DisplayStyle   style,
        M3dView::DisplayStatus  status) override;

    MStringArray getFilesToArchive(
        bool                    shortName,
        bool                    unresolvedName,
        bool                    markCouldBeImageSequence) const override;

    void getExternalContent(MExternalContentInfoTable& table) const override;
    void setExternalContent(const MExternalContentLocationTable& table) override;

  private:
    static MObject m_map;
    static MObject m_intensity;
    static MObject m_exposure;
    static MObject m_horizontalShift;
    static MObject m_verticalShift;
    static MObject m_message;
    static MObject m_displaySize;
};

class SkyDomeLightData
  : public MUserData
{
  public:
    SkyDomeLightData();

    float   m_size;
    MColor  m_color;
};

class SkyDomeLightDrawOverride
  : public MHWRender::MPxDrawOverride
{
  public:
    static MHWRender::MPxDrawOverride* creator(const MObject& obj);

    SkyDomeLightDrawOverride(const MObject& obj);

    MHWRender::DrawAPI supportedDrawAPIs() const override;

    bool isBounded(const MDagPath& objPath, const MDagPath& cameraPath) const override;

    MBoundingBox boundingBox(
        const MDagPath&                 objPath,
        const MDagPath&                 cameraPath) const override;

    MUserData* prepareForDraw(
        const MDagPath&                 objPath,
        const MDagPath&                 cameraPath,
        const MHWRender::MFrameContext& frameContext,
        MUserData*                      oldData) override;

    static void draw(
        const MHWRender::MDrawContext&  context,
        const MUserData*                data);
};

