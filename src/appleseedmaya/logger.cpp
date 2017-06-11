
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

// interface header.
#include "logger.h"

// Standard headers.
#include <cstdlib>
#include <cstring>

// Maya headers.
#include <maya/MGlobal.h>
#include <maya/MStatus.h>
#include "appleseedmaya/_endmayaheaders.h"

// appleseed.foundation headers.
#include "foundation/utility/log.h"
#include "foundation/utility/string.h"

namespace asf = foundation;
namespace asr = renderer;

namespace Logger
{
namespace
{

class LogTarget
  : public asf::ILogTarget
{
  public:
    virtual void release()
    {
        delete this;
    }

    virtual void write(
        const asf::LogMessage::Category  category,
        const char*                      file,
        const size_t                     line,
        const char*                      header,
        const char*                      message)
    {
        switch (category)
        {
            case asf::LogMessage::Debug:
                MGlobal::displayInfo(MString("[Debug]") + MString(message));
            break;

            case asf::LogMessage::Info:
                MGlobal::displayInfo(MString("[Info]") + message);
            break;

            case asf::LogMessage::Warning:
                MGlobal::displayWarning(MString("[Warning]") + message);
            break;

            case asf::LogMessage::Error:
            case asf::LogMessage::Fatal:
            default:
                MGlobal::displayError(MString("[Error]") + message);
            break;
        }
    }
};

LogTarget gLogTarget;

} // unnamed namespace.

MStatus initialize()
{
    asr::global_logger().add_target(&gLogTarget);

    asf::LogMessage::Category level = asf::LogMessage::Info;
    if (const char *logLevel = getenv("APPLESEED_MAYA_LOG_LEVEL"))
    {
        if (strcmp(logLevel, "debug") == 0)
            level = asf::LogMessage::Debug;
        else if (strcmp(logLevel, "info") == 0)
            level = asf::LogMessage::Info;
        else if (strcmp(logLevel, "warning") == 0)
            level = asf::LogMessage::Warning;
        else if (strcmp(logLevel, "error") == 0)
            level = asf::LogMessage::Error;
    }

    asr::global_logger().set_verbosity_level(level);
    return MS::kSuccess;
}

MStatus uninitialize()
{
    asr::global_logger().remove_target(&gLogTarget);
    return MS::kSuccess;
}

} // namespace Logger.

ScopedSetLoggerVerbosity::ScopedSetLoggerVerbosity(foundation::LogMessage::Category newLevel)
{
    m_prevLevel = asr::global_logger().get_verbosity_level();
    asr::global_logger().set_verbosity_level(newLevel);
}

ScopedSetLoggerVerbosity::~ScopedSetLoggerVerbosity()
{
    asr::global_logger().set_verbosity_level(m_prevLevel);
}
