
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

// interface header.
#include "logger.h"

// Standard headers.
#include <iostream>

// Maya headers.
#include <maya/MGlobal.h>

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
                MGlobal::displayInfo(message);
            break;

            case asf::LogMessage::Warning:
                MGlobal::displayWarning(message);
            break;

            case asf::LogMessage::Error:
            case asf::LogMessage::Fatal:
            default:
                MGlobal::displayError(message);
            break;
        }
    }
};

LogTarget gLogTarget;

} // unnamed namespace.

MStatus initialize()
{
    asr::global_logger().add_target(&gLogTarget);

#ifndef NDEBUG
    // while debugging we want all output...
    asr::global_logger().set_verbosity_level(asf::LogMessage::Debug);
#endif

    return MS::kSuccess;
}

MStatus uninitialize()
{
    asr::global_logger().remove_target(&gLogTarget);
    return MS::kSuccess;
}

} // namespace Logger.
