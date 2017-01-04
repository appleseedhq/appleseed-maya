
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

#ifndef APPLESEED_MAYA_PHYSICAL_SKY_LIGHT_NODE_H
#define APPLESEED_MAYA_PHYSICAL_SKY_LIGHT_NODE_H

// Maya headers.
#include <maya/MObject.h>
#include <maya/MPxLocatorNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

// appleseed.maya headers.
#include "appleseedmaya/envlightnode.h"

class PhysicalSkyLightNode
  : public EnvLightNode
{
  public:
    static const MString nodeName;
    static const MTypeId id;
    static const MString drawDbClassification;
    static const MString drawRegistrantId;

    static void* creator();
    static MStatus initialize();

  private:
    static MObject m_sunTheta;
    static MObject m_sunPhi;
    static MObject m_turbidity;
    static MObject m_turbidityMultiplier;
    static MObject m_luminanceMultiplier;
    static MObject m_luminanceGamma;
    static MObject m_saturationMultiplier;
    static MObject m_horizonShift;
    static MObject m_groundAlbedo;
    static MObject m_sunEnable;
    static MObject m_radianceMultiplier;
};

class PhysicalSkyLightData
  : public EnvLightData
{
  public:

    PhysicalSkyLightData();
};

class PhysicalSkyLightDrawOverride
  : public EnvLightDrawOverride
{
  public:

    static MHWRender::MPxDrawOverride *creator(const MObject& obj);

    PhysicalSkyLightDrawOverride(const MObject& obj);

    virtual MUserData *prepareForDraw(
        const MDagPath&                 objPath,
        const MDagPath&                 cameraPath,
        const MHWRender::MFrameContext& frameContext,
        MUserData*                      oldData);
};

#endif  // !APPLESEED_MAYA_PHYSICAL_SKY_LIGHT_NODE_H
