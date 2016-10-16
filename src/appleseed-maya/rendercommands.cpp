
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
#include "rendercommands.h"

// appleseed-maya headers.
#include "status.h"

// Maya headers.
#include <maya/MArgDatabase.h>
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MSyntax.h>


namespace
{

MObject getAppleseedGlobalsNode()
{
    /*
    MObject obj;
    MStatus stat;
    MSelectionList list;

    // Attempt to add the given name to the selection list,
    // then get the corresponding dependency node handle.
    if (!list.add(name) || !list.getDependNode(0, obj))
        return MObject();

    // Successful.
    stat = MStatus::kSuccess;
    return obj;
    */
    return MObject();
}

} // unnamed


MString FinalRenderCommand::cmdName("appleseedRender");

MSyntax FinalRenderCommand::syntaxCreator()
{
    MSyntax syntax;
    syntax.addFlag("-c", "-camera", MSyntax::kString);
    syntax.addFlag("-w", "-width", MSyntax::kLong);
    syntax.addFlag("-h", "-height", MSyntax::kLong);
    syntax.addFlag("-o", "-option", MSyntax::kString);
    return syntax;
}

void* FinalRenderCommand::creator()
{
    return new FinalRenderCommand();
}

MStatus FinalRenderCommand::doIt(const MArgList& args)
{
    return MStatus::kSuccess;
}

MString ProgressiveRenderCommand::cmdName("appleseedProgressiveRender");

MSyntax ProgressiveRenderCommand::syntaxCreator()
{
    MSyntax syntax;
    return syntax;
}

void* ProgressiveRenderCommand::creator()
{
    return new ProgressiveRenderCommand();
}

MStatus ProgressiveRenderCommand::doIt(const MArgList& args)
{
    return MStatus::kSuccess;
}
