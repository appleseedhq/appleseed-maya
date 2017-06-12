
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

#ifndef APPLESEED_MAYA_SESSION_H
#define APPLESEED_MAYA_SESSION_H

// Standard headers.
#include <set>

// Maya headers.
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include "appleseedmaya/_endmayaheaders.h"

// appleseed.maya headers.
#include "appleseedmaya/exporters/alphamapexporterfwd.h"
#include "appleseedmaya/exporters/shadingengineexporterfwd.h"
#include "appleseedmaya/exporters/shadingnetworkexporterfwd.h"
#include "appleseedmaya/utils.h"

// appleseed.foundation headers.
#include "foundation/core/concepts/noncopyable.h"

namespace AppleseedSession
{

MStatus initialize(const MString& pluginPath);
MStatus uninitialize();

enum SessionMode
{
    NoSession,
    ExportSession,
    FinalRenderSession,
    ProgressiveRenderSession
};

struct Options
{
    Options();

    // Common options.
    MString     m_camera;
    bool        m_selectionOnly;

    int         m_width;
    int         m_height;
    bool        m_renderRegion;
    int         m_xmin;
    int         m_ymin;
    int         m_xmax;
    int         m_ymax;

    const char* m_colorspace;

    // Final render options.
    // ...

    // IPR options.
    // ...

    // Project export options.
    bool        m_sequence;
    int         m_firstFrame;
    int         m_lastFrame;
    int         m_frameStep;
};

struct MotionBlurTimes
{
    MotionBlurTimes();

    void clear();

    void initializeToCurrentFrame();

    void initializeFrameSet(
        const size_t        numSamples,
        const float         shutterOpenTime,
        const float         shutterCloseTime,
        std::set<float>&    times);

    void mergeTimes();

    float normalizedFrame(float frame) const;

    float            m_shutterOpenTime;
    float            m_shutterCloseTime;

    std::set<float>  m_cameraTimes;
    std::set<float>  m_transformTimes;
    std::set<float>  m_deformTimes;

    std::set<float>  m_allTimes;
};

class Services
  : public foundation::NonCopyable
{
  public:

    virtual ~Services();

    virtual ShadingEngineExporterPtr createShadingEngineExporter(const MObject& object) const = 0;

    virtual ShadingNetworkExporterPtr createShadingNetworkExporter(
        const ShadingNetworkContext   context,
        const MObject&                object,
        const MPlug&                  outputPlug) const = 0;

    virtual AlphaMapExporterPtr createAlphaMapExporter(
        const MObject&                object) const = 0;

  protected:

    Services();
};

MStatus projectExport(const MString& fileName, Options options);

MStatus render(Options options);

MStatus batchRender(Options options);

void endSession();

SessionMode sessionMode();

const Options& options();

} // namespace AppleseedSession.

#endif  // !APPLESEED_MAYA_SESSION_H
