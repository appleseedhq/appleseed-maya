
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

#ifndef APPLESEED_MAYA_RENDERGLOBALSNODE_H
#define APPLESEED_MAYA_RENDERGLOBALSNODE_H

// appleseed-maya headers.
#include "appleseedmaya/appleseedsession.h"

// Maya headers.
#include <maya/MObject.h>
#include <maya/MPxNode.h>
#include <maya/MStringArray.h>
#include <maya/MTypeId.h>
#include "appleseedmaya/_endmayaheaders.h"

// Forward declarations.
namespace renderer { class Project; }

class RenderGlobalsNode
  : public MPxNode
{
  public:
    static const MString nodeName;
    static const MTypeId id;

    static void* creator();
    static MStatus initialize();

    MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;

    static void applyGlobalsToProject(
        const MObject&                      globals,
        renderer::Project&                  project);

    static void collectMotionBlurTimes(
        const MObject&                      globals,
        AppleseedSession::MotionBlurTimes&  motionBlurTimes);

  private:
    static MObject m_pixelSamples;
    static MObject m_passes;
    static MObject m_tileSize;

    static MObject m_lightingEngine;

    static MObject      m_diagnosticShader;
    static MStringArray m_diagnosticShaderKeys;

    static MObject m_limitBounces;
    static MObject m_globalBounces;
    static MObject m_specularBounces;
    static MObject m_glossyBounces;
    static MObject m_diffuseBounces;

    static MObject m_lightSamples;
    static MObject m_envSamples;
    static MObject m_caustics;
    static MObject m_maxRayIntensity;

    static MObject m_envLightNode;
    static MObject m_backgroundEmitsLight;

    static MObject m_motionBlur;
    static MObject m_mbCameraSamples;
    static MObject m_mbTransformSamples;
    static MObject m_mbDeformSamples;
    static MObject m_shutterOpen;
    static MObject m_shutterClose;

    static MObject m_renderingThreads;

    static MObject m_imageFormat;
};

#endif  // !APPLESEED_MAYA_RENDERGLOBALSNODE_H
