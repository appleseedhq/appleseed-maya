
#
# This source file is part of appleseed.
# Visit https://appleseedhq.net/ for additional information and resources.
#
# This software is released under the MIT license.
#
# Copyright (c) 2016-2018 Esteban Tovagliari, The appleseedhq Organization
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
import maya.cmds as mc
import maya.mel as mel
import maya.OpenMaya as om

# appleseedMaya imports.
from AETemplates import appleseedAETemplateCallback
from hypershadeCallbacks import *
from logger import logger
from menu import createMenu, deleteMenu
from renderer import createRenderMelProcedures
from renderGlobals import (
    createRenderTabsMelProcedures,
    renderSettingsBuiltCallback,
    addRenderGlobalsScriptJobs,
    removeRenderGlobalsScriptJobs)
from translator import createTranslatorMelProcedures


thisDir = os.path.normpath(os.path.dirname(__file__))

asXGenCallbacks = [
    ("RenderAPIRendererTabUIInit", "appleseedMaya.xgenseedui.xgseedUI"),
    ("RenderAPIRendererTabUIRefresh", "appleseedMaya.xgenseedui.xgseedRefresh"),
    ("PostDescriptionCreate", "appleseedMaya.xgenseedui.xgseedOnCreateDescription"),
    ("ArchiveExport", "appleseedMaya.xgenseed.xgseedArchiveExport"),
    ("ArchiveExportInfo", "appleseedMaya.xgenseed.xgseedArchiveExportInfo"),
    ("ArchiveExportInit", "appleseedMaya.xgenseed.xgseedArchiveExportInit")
]


def register():
    logger.info("Registering appleseed renderer.")

    # Register render.
    pm.renderer("appleseed", rendererUIName="appleseed")
    createRenderMelProcedures()

    # Final Render procedures.
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
    renderSettingsBuiltCallback('appleseed')

    pm.renderer("appleseed", edit=True, addGlobalsNode="defaultRenderGlobals")
    pm.renderer("appleseed", edit=True, addGlobalsNode="defaultResolution")
    pm.renderer(
        "appleseed", edit=True, addGlobalsNode="appleseedRenderGlobals")

    pm.callbacks(
        addCallback=renderSettingsBuiltCallback,
        hook="renderSettingsBuilt",
        owner="appleseed")

    addRenderGlobalsScriptJobs()

    # AE templates.
    pm.callbacks(
        addCallback=appleseedAETemplateCallback,
        hook="AETemplateCustomContent",
        owner="appleseed")

    # Manually load templates in AETemplates folder.
    templatesDir = os.path.join(thisDir, "AETemplates")
    logger.debug("Registering AETemplates in %s" % templatesDir)
    for file in os.listdir(templatesDir):
        if fnmatch.fnmatch(file, '*Template.py'):
            templateModule = file.replace(".py", "")
            logger.debug("Registering AE template %s" % templateModule)
            mel.eval('python("import appleseedMaya.AETemplates.%s")' % templateModule)

    # Hypershade callbacks
    asHypershadeCallbacks = [
        ("hyperShadePanelBuildCreateMenu", hyperShadePanelBuildCreateMenuCallback),
        ("hyperShadePanelBuildCreateSubMenu", hyperShadePanelBuildCreateSubMenuCallback),
        ("hyperShadePanelPluginChange", hyperShadePanelPluginChangeCallback),
        ("createRenderNodeSelectNodeCategories", createRenderNodeSelectNodeCategoriesCallback),
        ("createRenderNodePluginChange", createRenderNodePluginChangeCallback),
        ("renderNodeClassification", renderNodeClassificationCallback),
        ("createRenderNodeCommand", createRenderNodeCallback),
        ("nodeCanBeUsedAsMaterial", nodeCanBeUsedAsMaterialCallback),
        ("buildRenderNodeTreeListerContent", buildRenderNodeTreeListerContentCallback)
    ]
    for h, c in asHypershadeCallbacks:
        logger.debug("Adding {0} callback.".format(h))
        pm.callbacks(addCallback=c, hook=h, owner="appleseed")

    # appleseed translator.
    createTranslatorMelProcedures()

    # Logos.
    pm.renderer(
        "appleseed",
        edit=True,
        logoImageName="appleseed.png"
    )

    mel.eval('''
        global proc appleseedLogoCallback()
        {
            evalDeferred("showHelp -absolute \\\"https://appleseedhq.net\\\"");
        }
        '''
             )

    pm.renderer(
        "appleseed",
        edit=True,
        logoCallbackProcedure="appleseedLogoCallback"
    )

    # Menu
    if om.MGlobal.mayaState() == om.MGlobal.kInteractive:
        createMenu()

    # XGen
    try:
        import xgenm as xg
        for h, c in asXGenCallbacks:
            xg.registerCallback(h, c)

        logger.info("appleseedMaya: initialized xgenseed")
    except Exception as e:
        logger.info(
            "appleseedMaya: could not initialize xgenseed. error = %s" % e)


def unregister():
    logger.info("Unregistering appleseed renderer.")

    # XGen
    try:
        import xgenm as xg

        for h, c in asXGenCallbacks:
            xg.deregisterCallback(h, c)

        logger.info("appleseedMaya: uninitialized xgenseed")
    except Exception as e:
        logger.info(
            "appleseedMaya: could not uninitialize xgenseed. error = %s" % e)

    if om.MGlobal.mayaState() == om.MGlobal.kInteractive:
        deleteMenu()

    pm.callbacks(clearCallbacks=True, owner="appleseed")

    removeRenderGlobalsScriptJobs()

    if pm.renderer("appleseed", q=True, ex=True):
        pm.renderer("appleseed", unregisterRenderer=True)
