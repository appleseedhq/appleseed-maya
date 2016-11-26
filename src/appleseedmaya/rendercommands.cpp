
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

// Interface header.
#include "appleseedmaya/rendercommands.h"

// Standard headers.
#include <iostream>

// Maya headers.
#include <maya/MArgDatabase.h>
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MSyntax.h>

// appleseed.maya headers.
#include "appleseedmaya/config.h"
#include "appleseedmaya/logger.h"


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
    if(!list.add(name) || !list.getDependNode(0, obj))
        return MObject();

    // Successful.
    stat = MS::kSuccess;
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
    syntax.addFlag("-w", "-width" , MSyntax::kLong);
    syntax.addFlag("-h", "-height", MSyntax::kLong);
    syntax.addFlag("-b", "-batch" , MSyntax::kString);
    return syntax;
}

void* FinalRenderCommand::creator()
{
    return new FinalRenderCommand();
}

MStatus FinalRenderCommand::doIt(const MArgList& args)
{
    std::cout << "Appleseed Render:\n";
    std::cout << "-----------------\n";

    MStatus status;
    MArgDatabase argData(syntax(), args, &status);

    int width = -1;
    if(argData.isFlagSet("-width", &status))
        status = argData.getFlagArgument("-width", 0, width);

    int height = -1;
    if(argData.isFlagSet("-height", &status))
        status = argData.getFlagArgument("-height", 0, height);

    MString cameraName;
    if(argData.isFlagSet("-camera", &status))
        status = argData.getFlagArgument("-camera", 0, cameraName);

    std::cout << std::endl;
    return MS::kSuccess;
}

MString ProgressiveRenderCommand::cmdName("appleseedProgressiveRender");

MSyntax ProgressiveRenderCommand::syntaxCreator()
{
    MSyntax syntax;
    syntax.addFlag("-c", "-camera", MSyntax::kString);
    syntax.addFlag("-w", "-width" , MSyntax::kLong);
    syntax.addFlag("-h", "-height", MSyntax::kLong);
    syntax.addFlag("-a", "-action" , MSyntax::kString);
    return syntax;
}

void* ProgressiveRenderCommand::creator()
{
    return new ProgressiveRenderCommand();
}

MStatus ProgressiveRenderCommand::doIt(const MArgList& args)
{
    std::cout << "Appleseed IPR:\n";
    std::cout << "--------------\n";

    MStatus status;
    MArgDatabase argData(syntax(), args, &status);

    int width = -1;
    if(argData.isFlagSet("-width", &status))
        status = argData.getFlagArgument("-width", 0, width);

    int height = -1;
    if(argData.isFlagSet("-height", &status))
        status = argData.getFlagArgument("-height", 0, height);

    MString cameraName;
    if(argData.isFlagSet("-camera", &status))
        status = argData.getFlagArgument("-camera", 0, cameraName);

    MString action;
    if(argData.isFlagSet("-action", &status))
        status = argData.getFlagArgument("-action", 0, action);

    if(action == "start")
    {
        // ...
    }
    else if(action == "stop")
    {
        // ...
    }
    else if(action == "refresh")
    {
        // ...
    }
    else if(action == "running")
    {
        // ...
    }
    else if(action == "pause")
    {
        // ...
    }
    else if(action == "region")
    {
        // ...
    }
    else if(action.length() == 0)
    {
        // default action here
    }
    else
    {
        MGlobal::displayError("appleseedProgressiveRender: Unknown action argument.");
        return MS::kFailure;
    }

    std::cout << std::endl;
    return MS::kSuccess;
}
