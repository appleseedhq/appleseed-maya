
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

#ifndef APPLESEED_MAYA_CONFIG_H
#define APPLESEED_MAYA_CONFIG_H

// Maya headers.
#include <maya/MGlobal.h>
#include <maya/MStatus.h>

// appleseed.maya headers.
#include "appleseedmaya/version.h"

// Windows dll exports
#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
    #define APPLESEED_MAYA_PLUGIN_EXPORT __declspec(dllexport)
#else
    #define APPLESEED_MAYA_PLUGIN_EXPORT
#endif

// Constants, ...
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

// Check status and display an error if it is not success. Return status.
#define APPLESEED_MAYA_RET_MSTATUS_ERROR_MSG(stat, msg) \
{\
    MStatus s_ = (stat);\
    if (s_ != MStatus::kSuccess)\
    {\
        MGlobal::displayError(MString(msg));\
    }\
    return s_;\
}

// If cond is false, display an error and return MStatus::kFailure.
#define APPLESEED_MAYA_ENFORCE_RET_FAILURE_MSG(cond, msg) \
{\
    bool cond_ = (cond);\
    if (!cond_)\
    {\
        MGlobal::displayError(MString(msg));\
        return MStatus::kFailure;\
    }\
}

#endif  // !APPLESEED_MAYA_CONFIG_H
