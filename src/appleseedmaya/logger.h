
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

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/log.h"

// appleseed.foundation headers.
#include "foundation/core/concepts/noncopyable.h"
#include "foundation/memory/autoreleaseptr.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MStatus.h>
#include "appleseedmaya/_endmayaheaders.h"

namespace Logger
{

MStatus initialize();
MStatus uninitialize();

} // namespace Logger

//
// RAII class to set / restore the logger verbosity.
//

class ScopedSetLoggerVerbosity
  : public foundation::NonCopyable
{
  public:
    explicit ScopedSetLoggerVerbosity(foundation::LogMessage::Category newLevel);
    ~ScopedSetLoggerVerbosity();

  private:
    foundation::LogMessage::Category m_prevLevel;
};

//
// Helper class to manage appleseed log targets in an exception safe way.
//

class ScopedLogTarget
  : public foundation::NonCopyable
{
  public :
    ScopedLogTarget();
    ~ScopedLogTarget();

    void setLogTarget(foundation::auto_release_ptr<foundation::ILogTarget> logTarget);

  private :
    foundation::auto_release_ptr<foundation::ILogTarget> m_logTarget;
};

