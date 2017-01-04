
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

// Interface header.
#include "appleseedmaya/rendercommands.h"

// Standard headers.
#include <iostream>

// Maya headers.
#include <maya/MArgDatabase.h>
#include <maya/MCommonRenderSettingsData.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MGlobal.h>
#include <maya/MRenderUtil.h>
#include <maya/MRenderView.h>
#include <maya/MSelectionList.h>
#include <maya/MSyntax.h>

// appleseed.maya headers.
#include "appleseedmaya/appleseedsession.h"
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/config.h"
#include "appleseedmaya/logger.h"

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

    // In case we were rendering.
    AppleseedSession::endSession();

    // Initialize options from the render globals.
    AppleseedSession::Options options;

    MCommonRenderSettingsData renderSettings;
    MRenderUtil::getCommonRenderSettings(renderSettings);

    options.m_width = renderSettings.width;
    options.m_height = renderSettings.height;

    MObject globalsNode;
    getDependencyNodeByName("defaultRenderGlobals", globalsNode);

    MFnDependencyNode depNodeFn(globalsNode);
    if (depNodeFn.findPlug("useRenderRegion").asBool())
    {
        options.m_renderRegion = true;
        AttributeUtils::get(depNodeFn, "left" , options.m_xmin);
        AttributeUtils::get(depNodeFn, "bot"  , options.m_ymin);
        AttributeUtils::get(depNodeFn, "right", options.m_xmax);
        AttributeUtils::get(depNodeFn, "top"  , options.m_ymax);
    }

    //options.m_selectionOnly = !renderSettings.renderAll;

    bool isBatch = false;

    MStatus status;
    MArgDatabase argData(syntax(), args, &status);
    if (argData.isFlagSet("-batch", &status))
    {
        isBatch = true;
        MString batchOptions;
        status = argData.getFlagArgument("-batch", 0, batchOptions);
        std::cout << "  batch = true" << std::endl;
        std::cout << "  options = " << batchOptions << std::endl;

        options.m_sequence = renderSettings.isAnimated();
        if (options.m_sequence)
        {
            options.m_firstFrame = static_cast<float>(renderSettings.frameStart.as(MTime::uiUnit()));
            options.m_lastFrame = static_cast<float>(renderSettings.frameEnd.as(MTime::uiUnit()));
            options.m_frameStep = renderSettings.frameBy;
        }
    }
    else
    {
        if (argData.isFlagSet("-width", &status))
        {
            status = argData.getFlagArgument("-width", 0, options.m_width);
            std::cout << "  width = " << options.m_width << std::endl;
        }

        if (argData.isFlagSet("-height", &status))
        {
            status = argData.getFlagArgument("-height", 0, options.m_height);
            std::cout << "  height = " << options.m_height << std::endl;
        }

        if (argData.isFlagSet("-camera", &status))
        {
            status = argData.getFlagArgument("-camera", 0, options.m_camera);
            std::cout << "  camera = " << options.m_camera << std::endl;
        }
    }

    AppleseedSession::finalRender(options, isBatch);

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
    // Maybe we should check and fail instead of asserting?
    //assert(MRenderView::doesRenderEditorExist());

    /*
    std::cout << "Appleseed IPR:\n";
    std::cout << "--------------\n";

    // In case we were rendering.
    AppleseedSession::endSession();

    AppleseedSession::Options options;

    MStatus status;
    MArgDatabase argData(syntax(), args, &status);

    if (argData.isFlagSet("-width", &status))
    {
        status = argData.getFlagArgument("-width", 0, options.m_width);
        std::cout << "  width = " << options.m_width << std::endl;
    }

    if (argData.isFlagSet("-height", &status))
    {
        status = argData.getFlagArgument("-height", 0, options.m_height);
        std::cout << "  height = " << options.m_height << std::endl;
    }

    if (argData.isFlagSet("-camera", &status))
    {
        status = argData.getFlagArgument("-camera", 0, options.m_camera);
        std::cout << "  camera = " << options.m_camera << std::endl;
    }

    MString action;
    if (argData.isFlagSet("-action", &status))
        status = argData.getFlagArgument("-action", 0, action);

    std::cout << "  action = " << action << std::endl;

    if (action == "start")
    {
        // ...
    }
    else if (action == "stop")
    {
        // ...
    }
    else if (action == "refresh")
    {
        // ...
    }
    else if (action == "running")
    {
        const int iprRunning =
            AppleseedSession::sessionMode() == AppleseedSession::ProgressiveRenderSession;
        setResult(iprRunning);
        std::cout << "  result = " << iprRunning << std::endl;
    }
    else if (action == "pause")
    {
        // ...
    }
    else if (action == "region")
    {
        // ...
    }
    else if (action.length() == 0)
    {
        // default action here
    }
    else
    {
        MGlobal::displayError("appleseedProgressiveRender: Unknown action argument.");
        return MS::kFailure;
    }

    std::cout << std::endl;
    */
    return MS::kSuccess;
}
