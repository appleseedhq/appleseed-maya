
#
# This source file is part of appleseed.
# Visit http://appleseedhq.net/ for additional information and resources.
#
# This software is released under the MIT license.
#
# Copyright (c) 2016-2017 Esteban Tovagliari, The appleseedhq Organization
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

# Standard imports.
import os
import fnmatch

# Maya imports.
import pymel.core as pm
import maya.mel as mel
import maya.OpenMaya as om

# appleseedMaya imports.
from aetemplate import appleseedAETemplateCallback
from hyperShadeCallbacks import *
from logger import logger
from menu import createMenu, deleteMenu
from renderer import createRenderMelProcedures
from renderGlobals import createRenderTabsMelProcedures
from translator import createTranslatorMelProcedures
from util import appleseedIconsPath


thisDir = os.path.normpath(os.path.dirname(__file__))

def register():
    logger.info("Registering appleseed renderer.")

    # Register render.
    pm.renderer("appleseed", rendererUIName="appleseed")

    # Final Render procedures.
    createRenderMelProcedures()
    pm.renderer(
        "appleseed",
        edit=True,
        renderProcedure="appleseedRenderProcedure",
        commandRenderProcedure="appleseedBatchRenderProcedure",
        batchRenderProcedure="appleseedBatchRenderProcedure",
        cancelBatchRenderProcedure="appleseedCancelBatchRenderProcedure",
        renderRegionProcedure="mayaRenderRegion"
        )

    # Ipr Render procedures.
    pm.renderer(
        "appleseed",
        edit=True,
        iprRenderProcedure="appleseedIprRenderProcedure",
        isRunningIprProcedure="appleseedIsRunningIprRenderProcedure",
        startIprRenderProcedure="appleseedStartIprRenderProcedure",
        stopIprRenderProcedure="appleseedStopIprRenderProcedure",
        refreshIprRenderProcedure="appleseedRefreshIprRenderProcedure",
        pauseIprRenderProcedure="appleseedPauseIprRenderProcedure",
        changeIprRegionProcedure="appleseedChangeIprRegionProcedure",
        )

    # Globals
    createRenderTabsMelProcedures()
    pm.renderer("appleseed", edit=True, addGlobalsNode="defaultRenderGlobals")
    pm.renderer("appleseed", edit=True, addGlobalsNode="defaultResolution")
    pm.renderer("appleseed", edit=True, addGlobalsNode="appleseedRenderGlobals")

    pm.renderer(
        "appleseed",
        edit=True,
        addGlobalsTab=(
            "Common",
            "createMayaSoftwareCommonGlobalsTab",
            "updateMayaSoftwareCommonGlobalsTab"
            )
        )

    pm.renderer(
        "appleseed",
        edit=True,
        addGlobalsTab=(
            "Appleseed",
            "appleseedCreateTabProcedure",
            "appleseedUpdateTabProcedure"
            )
        )

    # AE templates.
    pm.callbacks(
        addCallback=appleseedAETemplateCallback,
        hook="AETemplateCustomContent",
        owner="appleseed")

    # Manually load templates in aetemplate folder.
    templatesDir =  os.path.join(thisDir, "aetemplate")
    logger.debug("Registering AETemplates in %s" % templatesDir)
    for file in os.listdir(templatesDir):
        if fnmatch.fnmatch(file, '*template.py'):
            templateModule = file.replace(".py", "")
            logger.debug("Registering AE template %s" % templateModule)
            mel.eval('python("import appleseedMaya.aetemplate.%s")' % templateModule)

    # Hypershader callbacks
    pm.callbacks(
        addCallback=hyperShadePanelBuildCreateMenuCallback,
        hook="hyperShadePanelBuildCreateMenu",
        owner="appleseed"
    )
    pm.callbacks(
        addCallback=hyperShadePanelBuildCreateSubMenuCallback,
        hook="hyperShadePanelBuildCreateSubMenu",
        owner="appleseed"
    )
    pm.callbacks(
        addCallback=buildRenderNodeTreeListerContentCallback,
        hook='buildRenderNodeTreeListerContent',
        owner="appleseed"
    )

    pm.callbacks(
        addCallback=createRenderNodeCallback,
        hook='createRenderNodeCommand',
        owner="appleseed")
    pm.callbacks(
        addCallback=connectNodeToNodeOverrideCallback,
        hook='connectNodeToNodeOverrideCallback',
        owner="appleseed")

    # Appleseed translator.
    createTranslatorMelProcedures()

    # Logos.
    if appleseedIconsPath():
        pm.renderer(
            "appleseed",
            edit=True,
            logoImageName=os.path.join(appleseedIconsPath(), "appleseed.png")
        )

        mel.eval('''
            global proc appleseedLogoCallback()
            {
                evalDeferred("showHelp -absolute \\\"http://appleseedhq.net\\\"");
            }
            '''
        )

        pm.renderer(
            "appleseed",
            edit=True,
            logoCallbackProcedure="appleseedLogoCallback"
        )
    else:
        logger.info("appleseedMaya: skipping logo registration. Logo not found")

    # Menu
    if om.MGlobal.mayaState() == om.MGlobal.kInteractive:
        createMenu()

    # XGen
    try:
        import xgenm as xg

        xg.registerCallback("RenderAPIRendererTabUIInit"   , "appleseedMaya.xgenseedui.xgseedUI" )
        xg.registerCallback("RenderAPIRendererTabUIRefresh", "appleseedMaya.xgenseedui.xgseedRefresh" )
        xg.registerCallback("PostDescriptionCreate"        , "appleseedMaya.xgenseedui.xgseedOnCreateDescription" )

        xg.registerCallback("ArchiveExport"    , "appleseedMaya.xgenseed.xgseedArchiveExport")
        xg.registerCallback("ArchiveExportInfo", "appleseedMaya.xgenseed.xgseedArchiveExportInfo")
        xg.registerCallback("ArchiveExportInit", "appleseedMaya.xgenseed.xgseedArchiveExportInit")
        logger.info("appleseedMaya: initialized xgenseed")
    except Exception as e:
        logger.info("appleseedMaya: could not initialize xgenseed. error = %s" % e)

def unregister():
    logger.info("Unregistering appleseed renderer.")

    # XGen
    try:
        import xgenm as xg

        xg.deregisterCallback("RenderAPIRendererTabUIInit"   , "appleseedMaya.xgenseedui.xgseedUI" )
        xg.deregisterCallback("RenderAPIRendererTabUIRefresh", "appleseedMaya.xgenseedui.xgseedRefresh" )
        xg.deregisterCallback("PostDescriptionCreate"        , "appleseedMaya.xgenseedui.xgseedOnCreateDescription" )

        xg.deregisterCallback("ArchiveExport"    , "appleseedMaya.xgenseed.xgseedArchiveExport")
        xg.deregisterCallback("ArchiveExportInfo", "appleseedMaya.xgenseed.xgseedArchiveExportInfo")
        xg.deregisterCallback("ArchiveExportInit", "appleseedMaya.xgenseed.xgseedArchiveExportInit")
        logger.info("appleseedMaya: uninitialized xgenseed")
    except Exception as e:
        logger.info("appleseedMaya: could not uninitialize xgenseed. error = %s" % e)

    if om.MGlobal.mayaState() == om.MGlobal.kInteractive:
        deleteMenu()

    pm.callbacks(clearCallbacks=True, owner="appleseed")

    if pm.renderer("appleseed", q=True, ex=True):
        pm.renderer("appleseed", unregisterRenderer=True)
