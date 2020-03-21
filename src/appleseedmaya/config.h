
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

// appleseed-maya headers.
#include "appleseedmaya/logger.h"
#include "appleseedmaya/version.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MGlobal.h>
#include <maya/MStatus.h>
#include "appleseedmaya/_endmayaheaders.h"

// Windows DLL exports.
#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
    #define APPLESEED_MAYA_PLUGIN_EXPORT __declspec(dllexport)
#else
    #define APPLESEED_MAYA_PLUGIN_EXPORT
#endif

// Constants.
#define APPLESEED_MAYA_VENDOR_STRING            "appleseedhq"

// Error handling macros.
// We define our own macros because we can have more control doing it, and it's very little code.

// Check status and display an error if it is not success.
#define APPLESEED_MAYA_CHECK_MSTATUS_MSG(stat, msg) \
{\
    MStatus s_ = (stat);\
    if (s_ != MStatus::kSuccess) \
    {\
        MGlobal::displayError(MString(msg));\
    }\
}

// Check status, log and display an error if it is not success.
#define APPLESEED_MAYA_CHECK_MSTATUS_MSG_LOG(stat, msg) \
{\
    MStatus s_ = (stat);\
    if (s_ != MStatus::kSuccess) \
    {\
        RENDERER_LOG_ERROR("status error string = %s", s_.errorString().asChar());\
        MGlobal::displayError(MString(msg));\
    }\
}

// Check status. If it is not success, display an error and return status.
#define APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(stat, msg) \
{\
    MStatus s_ = (stat);\
    if (s_ != MStatus::kSuccess)\
    {\
        MGlobal::displayError(MString(msg));\
        return s_;\
    }\
}

// Check status. If it is not success, display an error, log and return status.
#define APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(stat, msg) \
{\
    MStatus s_ = (stat);\
    if (s_ != MStatus::kSuccess)\
    {\
        MGlobal::displayError(MString(msg));\
        RENDERER_LOG_ERROR("status error string = %s", status.errorString().asChar());\
        return s_;\
    }\
}
