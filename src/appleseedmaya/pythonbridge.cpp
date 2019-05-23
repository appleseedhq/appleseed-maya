
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

// Interface header.
#include "appleseedmaya/pythonbridge.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/project.h"

// Python headers.
#include "Python.h"

namespace asf = foundation;
namespace asr = renderer;

namespace
{
    // RAII class to ensure / release the python state.
    struct ScopedGilState
    {
        PyGILState_STATE state;

        ScopedGilState()
        {
            state = PyGILState_Ensure();
        }

        ~ScopedGilState()
        {
            PyGILState_Release(state);
        }
    };

    PyObject* gAppleseedMayaNamespace = nullptr;
    PyObject* gCurrentProjectKey = nullptr;
}

MStatus PythonBridge::initialize(const MString& pluginPath)
{
    ScopedGilState gilState;

    PyObject* appleseedMayaModule = PyImport_ImportModule("appleseedMaya");

    if (appleseedMayaModule == nullptr)
        return MS::kFailure;

    // Fetch appleseedMaya module namespace.
    gAppleseedMayaNamespace = PyModule_GetDict(appleseedMayaModule);
    PyModule_AddObject(appleseedMayaModule, "currentProject", Py_None);

    gCurrentProjectKey = PyString_FromString("currentProject");

    return MS::kSuccess;
}

MStatus PythonBridge::uninitialize()
{
    Py_DecRef(gCurrentProjectKey);

    return MS::kSuccess;
}

void PythonBridge::setCurrentProject(renderer::Project* project)
{
    ScopedGilState gilState;

    const uintptr_t ptr = asf::binary_cast<uintptr_t>(project);
#ifdef _WIN32
    PyObject* py_ptr = PyLong_FromUnsignedLongLong(ptr);
#else
    PyObject* py_ptr = PyLong_FromUnsignedLong(ptr);
#endif

    PyDict_SetItem(gAppleseedMayaNamespace, gCurrentProjectKey, py_ptr);
    Py_DECREF(py_ptr);
}

void PythonBridge::clearCurrentProject()
{
    ScopedGilState gilState;

    PyDict_SetItem(gAppleseedMayaNamespace, gCurrentProjectKey, Py_None);
}
