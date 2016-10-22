
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

// Maya headers.
#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include <maya/MSwatchRenderRegister.h>

// appleseed-maya headers.
#include "appleseedmaya/appleseedsession.h"
#if MAYA_API_VERSION >= 201600
    #include "appleseedmaya/hypershaderenderer.h"
#endif
#include "appleseedmaya/config.h"
#include "appleseedmaya/envlightnode.h"
#include "appleseedmaya/rendercommands.h"
#include "appleseedmaya/renderglobalsnode.h"
#include "appleseedmaya/shadingnoderegistry.h"
#include "appleseedmaya/swatchrenderer.h"


APPLESEED_MAYA_PLUGIN_EXPORT MStatus initializePlugin(MObject plugin)
{
    MFnPlugin fnPlugin(
        plugin,
        APPLESEED_MAYA_VENDOR_STRING,
        APPLESEED_MAYA_VERSION_STRING,
        "Any");

    MStatus status;

    status = fnPlugin.registerNode(
        RenderGlobalsNode::nodeName,
        RenderGlobalsNode::id,
        RenderGlobalsNode::creator,
        RenderGlobalsNode::initialize);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseed-maya: failed to register render globals node");

    status = fnPlugin.registerNode(
        EnvLightNode::nodeName,
        EnvLightNode::id,
        EnvLightNode::creator,
        EnvLightNode::initialize,
        MPxNode::kLocatorNode,
        &EnvLightNode::drawDbClassification);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseed-maya: failed to register env light locator");


    status = fnPlugin.registerCommand(
        FinalRenderCommand::cmdName,
        FinalRenderCommand::creator,
        FinalRenderCommand::syntaxCreator);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseed-maya: failed to register render command");

    status = fnPlugin.registerCommand(
        ProgressiveRenderCommand::cmdName,
        ProgressiveRenderCommand::creator,
        ProgressiveRenderCommand::syntaxCreator);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseed-maya: failed to register render command");

    // Make sure that the modules we need can be imported...
    //status = MGlobal::executePythonCommand("import appleseed", true, false);
    //status = MGlobal::executePythonCommand("import appleseed_maya", true, false);

    status = MGlobal::executePythonCommand("import appleseed_maya.register; appleseed_maya.register.register()", true, false);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseed-maya: failed to initialize renderer");

    status = ShadingNodeRegistry::registerShadingNodes(plugin);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseed-maya: failed to register shading nodes");

    if (MGlobal::mayaState() != MGlobal::kBatch)
    {
        status = MSwatchRenderRegister::registerSwatchRender(SwatchRenderer::name, SwatchRenderer::creator);
        APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
            status,
            "appleseed-maya: failed to register swatch renderer");
    }

#if MAYA_API_VERSION >= 201600
    status = fnPlugin.registerRenderer(HypershadeRenderer::name, HypershadeRenderer::creator);

    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseed-maya: failed to register hypershade renderer");
#endif

    AppleseedSession::instance().initialize();
    return status;
}

APPLESEED_MAYA_PLUGIN_EXPORT MStatus uninitializePlugin(MObject plugin)
{
    MFnPlugin fnPlugin(plugin);
    MStatus status;

#if MAYA_API_VERSION >= 201600
    status = fnPlugin.deregisterRenderer(HypershadeRenderer::name);
    APPLESEED_MAYA_CHECK_MSTATUS_MSG(
        status,
        "appleseed-maya: failed to deregister hypershade renderer");
#endif

    if (MGlobal::mayaState() != MGlobal::kBatch)
    {
        status = MSwatchRenderRegister::unregisterSwatchRender(SwatchRenderer::name);
        APPLESEED_MAYA_CHECK_MSTATUS_MSG(
            status,
            "appleseed-maya: failed to deregister swatch renderer");
    }

    status = ShadingNodeRegistry::unregisterShadingNodes(plugin);
    APPLESEED_MAYA_CHECK_MSTATUS_MSG(
        status,
        "appleseed-maya: failed to deregister shading nodes");

    status = fnPlugin.deregisterCommand(FinalRenderCommand::cmdName);
    APPLESEED_MAYA_CHECK_MSTATUS_MSG(
        status,
        "appleseed-maya: failed to deregister render command");

    status = fnPlugin.deregisterCommand(ProgressiveRenderCommand::cmdName);
    APPLESEED_MAYA_CHECK_MSTATUS_MSG(
        status,
        "appleseed-maya: failed to deregister render command");

    status = fnPlugin.deregisterNode(EnvLightNode::id);
    APPLESEED_MAYA_CHECK_MSTATUS_MSG(
        status,
        "appleseed-maya: failed to deregister env light locator");

    status = fnPlugin.deregisterNode(RenderGlobalsNode::id);
    APPLESEED_MAYA_CHECK_MSTATUS_MSG(
        status,
        "appleseed-maya: failed to deregister render globals node");

    status = MGlobal::executePythonCommand("import appleseed_maya.register; appleseed_maya.register.unregister()", true, false);
    APPLESEED_MAYA_CHECK_MSTATUS_MSG(
        status,
        "appleseed-maya: failed to uninitialize render");

    AppleseedSession::instance().uninitialize();
    return status;
}
