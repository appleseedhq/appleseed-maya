
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016 Haggi Krey, The appleseedhq Organization
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
#include "renderglobalsnode.h"

// appleseed-maya headers.
#include "status.h"
#include "typeids.h"

// appleseed.renderer headers.
#include "renderer/api/utility.h"

// Maya headers.


MString RenderGlobalsNode::nodeName("appleseedRenderGlobals");
MTypeId RenderGlobalsNode::id(RenderGlobalsNodeTypeId);

void* RenderGlobalsNode::creator()
{
    return new RenderGlobalsNode();
}

MStatus RenderGlobalsNode::initialize()
{
    return MStatus::kSuccess;
}

MStatus RenderGlobalsNode::compute(const MPlug& plug, MDataBlock& dataBlock)
{
    return MStatus::kSuccess;
}
