
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

// appleseed-maya headers.
#include "appleseedsession.h"
#if MAYA_API_VERSION >= 201600
    #include "hypershaderenderer.h"
#endif
#include "rendercommands.h"
#include "renderglobalsnode.h"
#include "status.h"
#include "shadingnoderegistry.h"
#include "swatchrenderer.h"

// Maya headers.
#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include <maya/MSwatchRenderRegister.h>


#ifdef _WIN32
__declspec(dllexport)
#endif
MStatus initializePlugin(MObject plugin)
{
    MFnPlugin fnPlugin(plugin, "appleseedhq", "0.1.0", "Any");
    MStatus status;

    status = fnPlugin.registerNode(
        RenderGlobalsNode::nodeName,
        RenderGlobalsNode::id,
        RenderGlobalsNode::creator,
        RenderGlobalsNode::initialize);

    status = fnPlugin.registerCommand(
        FinalRenderCommand::cmdName,
        FinalRenderCommand::creator,
        FinalRenderCommand::syntaxCreator);

    status = fnPlugin.registerCommand(
        ProgressiveRenderCommand::cmdName,
        ProgressiveRenderCommand::creator,
        ProgressiveRenderCommand::syntaxCreator);

    //status = MGlobal::executePythonCommand("import appleseed", true, false);
    //status = MGlobal::executePythonCommand("import appleseed_maya", true, false);

    status = MGlobal::executePythonCommand("import appleseed_maya.renderer; appleseed_maya.renderer.register()", true, false);

    status = ShadingNodeRegistry::registerShadingNodes(plugin);

    if (MGlobal::mayaState() != MGlobal::kBatch)
        status = MSwatchRenderRegister::registerSwatchRender(SwatchRenderer::name, SwatchRenderer::creator);

#if MAYA_API_VERSION >= 201600
    status = fnPlugin.registerRenderer(HypershadeRenderer::name, HypershadeRenderer::creator);
#endif

    AppleseedSession::instance().initialize();
    return status;
}

#ifdef _WIN32
__declspec(dllexport)
#endif
MStatus uninitializePlugin(MObject plugin)
{
    MFnPlugin fnPlugin(plugin);
    MStatus status;

#if MAYA_API_VERSION >= 201600
    status = fnPlugin.deregisterRenderer(HypershadeRenderer::name);
#endif

    if (MGlobal::mayaState() != MGlobal::kBatch)
        status = MSwatchRenderRegister::unregisterSwatchRender(SwatchRenderer::name);

    status = ShadingNodeRegistry::unregisterShadingNodes(plugin);

    status = fnPlugin.deregisterCommand(FinalRenderCommand::cmdName);
    status = fnPlugin.deregisterCommand(ProgressiveRenderCommand::cmdName);

    status = fnPlugin.deregisterNode(RenderGlobalsNode::id);

    status = MGlobal::executePythonCommand("import appleseed_maya.renderer; appleseed_maya.renderer.unregister()", true, false);

    AppleseedSession::instance().uninitialize();
    return status;
}
