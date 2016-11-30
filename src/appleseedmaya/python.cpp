
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

// Python headers.
#include <Python.h>

// Interface header.
#include "appleseedmaya/python.h"

// Boost headers.
#include "boost/python.hpp"

// appleseed.renderer headers.
#include "renderer/api/project.h"

namespace bpy = boost::python;
namespace asf = foundation;
namespace asr = renderer;

namespace
{

struct ScopedGilState
{
    ScopedGilState()
    {
        state = PyGILState_Ensure();
    }

    ~ScopedGilState()
    {
        PyGILState_Release(state);
    }

    PyGILState_STATE state;
};

bpy::object gAppleseedMayaNamespace;

} // unnamed

MStatus PythonBridge::initialize(const MString& pluginPath)
{
    ScopedGilState gilState;

    MStatus status;

    try
    {
        // Fetch appleseedMaya module namespace.
        bpy::object appleseedMayaModule = bpy::import("appleseedMaya");
        gAppleseedMayaNamespace = appleseedMayaModule.attr("__dict__");

        // Init the current project global (to None).
        gAppleseedMayaNamespace["currentProject"] = bpy::object();
    }
    catch(const bpy::error_already_set&)
    {
        PyErr_Print();
    }

    return status;
}

MStatus PythonBridge::uninitialize()
{
    MStatus status;
    return status;
}

void PythonBridge::setCurrentProject(renderer::Project *project)
{
    ScopedGilState gilState;
    gAppleseedMayaNamespace["currentProject"] = bpy::ptr(project);
}

void PythonBridge::clearCurrentProject()
{
    ScopedGilState gilState;
    gAppleseedMayaNamespace["currentProject"] = bpy::object();
}
