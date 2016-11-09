
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

#ifndef APPLESEED_MAYA_SESSION_H
#define APPLESEED_MAYA_SESSION_H

// Maya headers.
#include <maya/MStatus.h>
#include <maya/MString.h>


class AppleseedSession
{
  public:

    static MStatus initialize(const MString& pluginPath);
    static MStatus uninitialize();

    enum SessionMode
    {
        NoSession,
        ExportSession,
        FinalRenderSession,
        ProgressiveRenderSession
    };

    struct Options
    {
        Options()
          : m_width(-1)
          , m_height(-1)
          , m_selectionOnly(false)
          , m_sequence(false)
        {
        }

        // Common options.
        int m_width;
        int m_height;
        MString m_camera;
        bool m_selectionOnly;

        // Final render options.
        // ...

        // IPR options.
        // ...

        // Project export options.
        bool m_sequence;
        int m_firstFrame;
        int m_lastFrame;
    };

    static void beginProjectExport(
        const MString& fileName,
        const Options& options);

    static void endProjectExport();

    static void beginFinalRender(
        const Options& options);

    static void endFinalRender();

    static void beginProgressiveRender(
        const Options& options);

    static void endProgressiveRender();

    static SessionMode mode();
    static const Options& options();
};

#endif  // !APPLESEED_MAYA_SESSION_H
