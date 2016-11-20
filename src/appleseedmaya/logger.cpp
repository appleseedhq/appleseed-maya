
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

namespace Logger
{

MStatus initialize()
{
    return MS::kSuccess;
}

MStatus uninitialize()
{
    return MS::kSuccess;
}

void debug(const char *msg)
{
    std::cout << "Debug: " << msg << std::endl;
}

void debug(const MString& msg)
{
    debug(msg.asChar());
}

void info(const char *msg)
{
    std::cout << "Info: " << msg << std::endl;
}

void info(const MString& msg)
{
    info(msg.asChar());
}

void warning(const char *msg)
{
    std::cout << "Warning: " << msg << std::endl;
}

void warning(const MString& msg)
{
    warning(msg.asChar());
}

void error(const char *msg)
{
    std::cout << "Error: " << msg << std::endl;
}

void error(const MString& msg)
{
    error(msg.asChar());
}

} // namespace Logger.
