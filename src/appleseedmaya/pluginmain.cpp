
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016-2018 Esteban Tovagliari, The appleseedhq Organization
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
#include "appleseedmaya/alphamapnode.h"
#include "appleseedmaya/appleseedsession.h"
#include "appleseedmaya/appleseedtranslator.h"
#include "appleseedmaya/config.h"
#include "appleseedmaya/exporters/exporterfactory.h"
#include "appleseedmaya/extensionattributes.h"
#include "appleseedmaya/hypershaderenderer.h"
#include "appleseedmaya/idlejobqueue.h"
#include "appleseedmaya/logger.h"
#include "appleseedmaya/pythonbridge.h"
#include "appleseedmaya/rendercommands.h"
#include "appleseedmaya/renderglobalsnode.h"
#include "appleseedmaya/shadingnoderegistry.h"
#include "appleseedmaya/swatchrenderer.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include <maya/MSwatchRenderRegister.h>
#include "appleseedmaya/_endmayaheaders.h"

// Must be last to avoid conflicts with symbols defined in X headers.
#include "appleseedmaya/physicalskylightnode.h"
#include "appleseedmaya/skydomelightnode.h"

// Constants.
static const char* gDrawRegistrantId = "appleseedMaya";

APPLESEED_MAYA_PLUGIN_EXPORT MStatus initializePlugin(MObject plugin)
{
    /***************************/
    // Plugin.

    MStatus status;
    MFnPlugin fnPlugin(
        plugin,
        APPLESEED_MAYA_VENDOR_STRING,
        APPLESEED_MAYA_VERSION_STRING,
        "Any",
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: failed to initialize MFnPlugin");

    const MString pluginPath = fnPlugin.loadPath(&status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
        status,
        "appleseedMaya: failed to get plugin path");

    status = Logger::initialize();
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: failed to initialize logger");

    RENDERER_LOG_INFO("Initializing appleseedMaya plugin");

    /***************************/
    // Nodes.

    status = fnPlugin.registerNode(
        RenderGlobalsNode::nodeName,
        RenderGlobalsNode::id,
        RenderGlobalsNode::creator,
        RenderGlobalsNode::initialize);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
        status,
        "appleseedMaya: failed to register render globals node");

    status = fnPlugin.registerNode(
        AlphaMapNode::nodeName,
        AlphaMapNode::id,
        AlphaMapNode::creator,
        AlphaMapNode::initialize);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
        status,
        "appleseedMaya: failed to register alpha map node");

    status = fnPlugin.registerNode(
        PhysicalSkyLightNode::nodeName,
        PhysicalSkyLightNode::id,
        PhysicalSkyLightNode::creator,
        PhysicalSkyLightNode::initialize,
        MPxNode::kLocatorNode,
        &PhysicalSkyLightNode::drawDbClassification);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
        status,
        "appleseedMaya: failed to register sky light locator");

    status = fnPlugin.registerNode(
        SkyDomeLightNode::nodeName,
        SkyDomeLightNode::id,
        SkyDomeLightNode::creator,
        SkyDomeLightNode::initialize,
        MPxNode::kLocatorNode,
        &SkyDomeLightNode::drawDbClassification);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
        status,
        "appleseedMaya: failed to register dome light locator");

    if (MGlobal::mayaState() == MGlobal::kInteractive)
    {
        status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(
            PhysicalSkyLightNode::drawDbClassification,
            gDrawRegistrantId,
            PhysicalSkyLightDrawOverride::creator);
        APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
            status,
            "appleseedMaya: failed to register sky light locator draw override");

        status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(
            SkyDomeLightNode::drawDbClassification,
            gDrawRegistrantId,
            SkyDomeLightDrawOverride::creator);
        APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
            status,
            "appleseedMaya: failed to register dome light locator draw override");
    }

    /***************************/
    // Commands.

    status = fnPlugin.registerCommand(
        FinalRenderCommand::cmdName,
        FinalRenderCommand::creator,
        FinalRenderCommand::syntaxCreator);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(
        status,
        "appleseedMaya: failed to register final render command");

    if (MGlobal::mayaState() == MGlobal::kInteractive)
    {
        status = fnPlugin.registerCommand(
            ProgressiveRenderCommand::cmdName,
            ProgressiveRenderCommand::creator,
            ProgressiveRenderCommand::syntaxCreator);
        APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
            status,
            "appleseedMaya: failed to register progressive render command");
    }

    /***************************/
    // Extension attributes.

    status = addExtensionAttributes();
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
        status,
        "appleseedMaya: failed to add extension attributes");

    /***************************/
    // Scripts.

    // Make sure that the modules we need can be imported.
    status = MGlobal::executePythonCommand("import appleseed", false, false);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
        status,
        "appleseedMaya: failed to import required Python modules");

    status = MGlobal::executePythonCommand("import appleseedMaya", false, false);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
        status,
        "appleseedMaya: failed to import required Python modules");

    status = MGlobal::executePythonCommand("import appleseedMaya.register; appleseedMaya.register.register()", false, false);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
        status,
        "appleseedMaya: failed to initialize renderer");

    /***************************/
    // Shading nodes & previews.

    status = ShadingNodeRegistry::registerShadingNodes(plugin);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
        status,
        "appleseedMaya: failed to register shading nodes");

    if (MGlobal::mayaState() == MGlobal::kInteractive)
    {
        SwatchRenderer::initialize(pluginPath);
        status = MSwatchRenderRegister::registerSwatchRender(
            SwatchRenderer::name,
            SwatchRenderer::creator);
        APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
            status,
            "appleseedMaya: failed to register swatch renderer");

        status = fnPlugin.registerRenderer(
            HypershadeRenderer::name,
            HypershadeRenderer::creator);

        APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
            status,
            "appleseedMaya: failed to register hypershade renderer");
    }

    /***************************/
    // Translators.

    status = fnPlugin.registerFileTranslator(
            AppleseedTranslator::translatorName,
            "",                                 // pixmap name
            AppleseedTranslator::creator,
            "appleseedTranslatorOpts",          // options display script name
            "",                                 // default options which are passed to the display script
            true);                              // can use MGlobal::executeCommand
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
        status,
        "appleseedMaya: failed to register appleseed translator");

    status = fnPlugin.registerFileTranslator(
            AppleseedzTranslator::translatorName,
            "",                                 // pixmap name
            AppleseedzTranslator::creator,
            "appleseedTranslatorOpts",          // options display script name
            "",                                 // default options which are passed to the display script
            true);                              // can use MGlobal::executeCommand
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
        status,
        "appleseedMaya: failed to register appleseedz translator");

    /***************************/
    // Internal.

    status = NodeExporterFactory::initialize(pluginPath);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
        status,
        "appleseedMaya: failed to initialize node exporters factory");

    status = AppleseedSession::initialize(pluginPath);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
        status,
        "appleseedMaya: failed to initialize session");

    status = PythonBridge::initialize(pluginPath);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
        status,
        "appleseedMaya: failed to initialize Python bridge");

    IdleJobQueue::initialize();

    RENDERER_LOG_INFO("Registration done!");
    return status;
}

APPLESEED_MAYA_PLUGIN_EXPORT MStatus uninitializePlugin(MObject plugin)
{
    /***************************/
    // Plugin.

    MStatus status;
    MFnPlugin fnPlugin(
        plugin,
        APPLESEED_MAYA_VENDOR_STRING,
        APPLESEED_MAYA_VERSION_STRING,
        "Any",
        &status);
    APPLESEED_MAYA_CHECK_MSTATUS_MSG_LOG(
        status,
        "appleseedMaya: failed to initialize MFnPlugin");

    RENDERER_LOG_INFO("Uninitializing appleseedMaya plugin");

    /***************************/
    // Internal.

    IdleJobQueue::uninitialize();

    status = AppleseedSession::uninitialize();
    APPLESEED_MAYA_CHECK_MSTATUS_MSG_LOG(
        status,
        "appleseedMaya: failed to uninitialize session");

    status = PythonBridge::uninitialize();
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG_LOG(
        status,
        "appleseedMaya: failed to unititialize Python bridge");

    status = NodeExporterFactory::uninitialize();
    APPLESEED_MAYA_CHECK_MSTATUS_MSG_LOG(
        status,
        "appleseedMaya: failed to uninitialize node exporters factory");

    /***************************/
    // Shading nodes & previews.

    if (MGlobal::mayaState() == MGlobal::kInteractive)
    {
        status = fnPlugin.deregisterRenderer(HypershadeRenderer::name);
        APPLESEED_MAYA_CHECK_MSTATUS_MSG_LOG(
            status,
            "appleseedMaya: failed to deregister hypershade renderer");

        SwatchRenderer::uninitialize();
        status = MSwatchRenderRegister::unregisterSwatchRender(SwatchRenderer::name);
        APPLESEED_MAYA_CHECK_MSTATUS_MSG_LOG(
            status,
            "appleseedMaya: failed to deregister swatch renderer");
    }

    status = ShadingNodeRegistry::unregisterShadingNodes(plugin);
    APPLESEED_MAYA_CHECK_MSTATUS_MSG_LOG(
        status,
        "appleseedMaya: failed to deregister shading nodes");

    /***************************/
    // Commands.

    status = fnPlugin.deregisterCommand(FinalRenderCommand::cmdName);
    APPLESEED_MAYA_CHECK_MSTATUS_MSG_LOG(
        status,
        "appleseedMaya: failed to deregister render command");

    if (MGlobal::mayaState() == MGlobal::kInteractive)
    {
        status = fnPlugin.deregisterCommand(ProgressiveRenderCommand::cmdName);
        APPLESEED_MAYA_CHECK_MSTATUS_MSG_LOG(
            status,
            "appleseedMaya: failed to deregister render command");
    }

    /***************************/
    // Nodes.

    status = fnPlugin.deregisterNode(PhysicalSkyLightNode::id);
    APPLESEED_MAYA_CHECK_MSTATUS_MSG_LOG(
        status,
        "appleseedMaya: failed to deregister sky light locator");

    status = fnPlugin.deregisterNode(SkyDomeLightNode::id);
    APPLESEED_MAYA_CHECK_MSTATUS_MSG_LOG(
        status,
        "appleseedMaya: failed to deregister dome light locator");

    status = fnPlugin.deregisterNode(AlphaMapNode::id);
    APPLESEED_MAYA_CHECK_MSTATUS_MSG_LOG(
        status,
        "appleseedMaya: failed to deregister alpha map node");

    status = fnPlugin.deregisterNode(RenderGlobalsNode::id);
    APPLESEED_MAYA_CHECK_MSTATUS_MSG_LOG(
        status,
        "appleseedMaya: failed to deregister render globals node");

    if (MGlobal::mayaState() == MGlobal::kInteractive)
    {
        status = MHWRender::MDrawRegistry::deregisterDrawOverrideCreator(
                SkyDomeLightNode::drawDbClassification,
                gDrawRegistrantId);
        APPLESEED_MAYA_CHECK_MSTATUS_MSG_LOG(
            status,
            "appleseedMaya: failed to deregister dome light locator draw override");

        status = MHWRender::MDrawRegistry::deregisterDrawOverrideCreator(
                PhysicalSkyLightNode::drawDbClassification,
                gDrawRegistrantId);
        APPLESEED_MAYA_CHECK_MSTATUS_MSG_LOG(
            status,
            "appleseedMaya: failed to deregister sky light locator draw override");
    }

    /***************************/
    // Translator.

    status = fnPlugin.deregisterFileTranslator(AppleseedTranslator::translatorName);
    APPLESEED_MAYA_CHECK_MSTATUS_MSG_LOG(
        status,
        "appleseedMaya: failed to deregister appleseed translator");

    status = fnPlugin.deregisterFileTranslator(AppleseedzTranslator::translatorName);
    APPLESEED_MAYA_CHECK_MSTATUS_MSG_LOG(
        status,
        "appleseedMaya: failed to deregister appleseedz translator");

    /***************************/
    // Scripts.

    status = MGlobal::executePythonCommand("import appleseedMaya.register; appleseedMaya.register.unregister()", false, false);
    APPLESEED_MAYA_CHECK_MSTATUS_MSG_LOG(
        status,
        "appleseedMaya: failed to uninitialize render");

    /***************************/
    // Logger.

    status = Logger::uninitialize();
    APPLESEED_MAYA_CHECK_MSTATUS_MSG_LOG(
        status,
        "appleseedMaya: failed to uninitialize logger");

    RENDERER_LOG_INFO("Unregister done!");

    return status;
}
