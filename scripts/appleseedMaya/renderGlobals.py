
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

# Maya imports.
import maya.cmds as mc
import maya.mel as mel
import pymel.core as pm

# appleseedMaya imports.
from logger import logger


def createGlobalNodes():
    if mc.objExists("appleseedRenderGlobals"):
        return

    sel = mc.ls(sl=True)
    mc.createNode(
        "appleseedRenderGlobals",
        name="appleseedRenderGlobals",
        shared=True,
        skipSelect=True)
    mc.lockNode("appleseedRenderGlobals")
    mc.select(sel, replace=True)
    logger.debug("Created appleseed render global node")

def imageFormatChanged():
    # Since we only support two file formats atm., we can hardcode things.
    # 32 is the format code for png, 51 is custom image format.
    # We also update the extension attribute (used in the file names preview).
    newFormat = mc.getAttr("appleseedRenderGlobals.imageFormat")

    if newFormat == 0: # EXR
        mc.setAttr("defaultRenderGlobals.imageFormat", 51)
        mc.setAttr("defaultRenderGlobals.imfkey", "exr", type="string")
    elif newFormat == 1: # PNG
        mc.setAttr("defaultRenderGlobals.imageFormat", 32)
        mc.setAttr("defaultRenderGlobals.imfkey", "png", type="string")
    else:
        raise RuntimeError("Unknown render global image file format")

def currentRendererChanged():
    if mel.eval("currentRenderer()") != "appleseed":
        return

    # Make sure our render globals node exists.
    createGlobalNodes()

    # If the render globals window does not exist, create it.
    if not mc.window("unifiedRenderGlobalsWindow", exists=True):
        mel.eval("unifiedRenderGlobalsWindow")
        mc.window("unifiedRenderGlobalsWindow", edit=True, visible=False)

    # "Customize" the image formats menu.
    mc.setParent("unifiedRenderGlobalsWindow")
    mel.eval("setParentToCommonTab;")
    mc.setParent("imageFileOutputSW")
    mc.setParent("imageMenuMayaSW")
    mc.setParent("..")
    parent = mc.setParent(q=True)

    # Remove the menu callback and the menu items.
    mel.eval('optionMenuGrp -e -changeCommand "" imageMenuMayaSW;')
    items = mc.optionMenuGrp("imageMenuMayaSW", q=True, itemListLong=True)
    for item in items:
        mc.deleteUI(item)

    # Add the formats we support.
    menu = parent + "|imageMenuMayaSW|OptionMenu"
    mc.menuItem(parent=menu, label="OpenEXR (.exr)", data=0)
    mc.menuItem(parent=menu, label="PNG (.png)", data=1)

    # Connect the control to one internal attribute in our globals node
    # so that we can add a changed callback to it.
    mc.connectControl("imageMenuMayaSW", "appleseedRenderGlobals.imageFormat", index=1)
    mc.connectControl("imageMenuMayaSW", "appleseedRenderGlobals.imageFormat", index=2)

    # Add a callback when our internal attribute changes.
    # This callback gets the current value from our internal attribute and
    # uses it to update the original image format attribute (closing the circle.)
    mc.scriptJob(
        parent=parent,
        replacePrevious=True,
        attributeChange=[
            "appleseedRenderGlobals.imageFormat",
            "from appleseedMaya.renderGlobals import imageFormatChanged; imageFormatChanged()"]
    )

    # Update the image format controls now.
    imageFormatChanged()

def postUpdateCommonTab():
    imageFormatChanged()

class AppleseedRenderGlobalsMainTab(object):
    def __init__(self):
        self.__uis = {}
        self.__scriptJobs = {}

    def __addControl(self, ui, attrName, changeCallback=None, connectIndex=2):
        self.__uis[attrName] = ui
        attr = pm.Attribute("appleseedRenderGlobals." + attrName)
        pm.connectControl(ui, attr, index=connectIndex)

        if changeCallback:
            self.__scriptJobs[attrName] = mc.scriptJob(
                attributeChange=["appleseedRenderGlobals." + attrName, changeCallback])

    def create(self):
        # Create default render globals node if needed.
        createGlobalNodes()

        parentForm = pm.setParent(query=True)
        pm.setUITemplate("renderGlobalsTemplate", pushTemplate=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)

        columnWidth = 400

        with pm.scrollLayout("appleseedScrollLayout", horizontalScrollBarThickness=0):
            with pm.columnLayout("appleseedColumnLayout", adjustableColumn=True, width=columnWidth):
                with pm.frameLayout(label="Sampling", collapsable=True, collapse=False):
                    with pm.columnLayout("appleseedColumnLayout", adjustableColumn=True, width=columnWidth):
                        self.__addControl(
                            ui=pm.intFieldGrp(label="Pixel Samples", numberOfFields = 1),
                            attrName="samples")
                        self.__addControl(
                            ui=pm.intFieldGrp(label="Render Passes", numberOfFields = 1),
                            attrName="passes")
                        self.__addControl(
                            ui=pm.intFieldGrp(label="Tile Size", numberOfFields = 1),
                            attrName="tileSize")

                with pm.frameLayout(label="Shading", collapsable=True, collapse=False):
                    with pm.columnLayout("appleseedColumnLayout", adjustableColumn=True, width=columnWidth):
                        attr = pm.Attribute("appleseedRenderGlobals.diagnostics")
                        menuItems = [(i, v) for i, v in enumerate(attr.getEnums().keys())]
                        self.__addControl(
                            ui=pm.attrEnumOptionMenuGrp(label="Override Shaders", enumeratedItem=menuItems),
                            attrName = "diagnostics")

                with pm.frameLayout(label="Lighting", collapsable=True, collapse=False):
                    with pm.columnLayout("appleseedColumnLayout", adjustableColumn=True, width=columnWidth):
                        self.__addControl(
                            ui=pm.checkBoxGrp(label="Global Illumination"),
                            attrName="gi")
                        self.__addControl(
                            ui=pm.checkBoxGrp(label="Caustics"),
                            attrName="caustics")
                        self.__addControl(
                            ui=pm.intFieldGrp(label="GI Bounces", numberOfFields = 1),
                            attrName="bounces")

                        self.__addControl(
                            ui=pm.floatFieldGrp(label="Max Ray Intensity", numberOfFields = 1),
                            attrName="maxRayIntensity")

                        self.__addControl(
                            ui=pm.floatFieldGrp(label="Light Samples", numberOfFields = 1),
                            attrName="lightSamples")
                        self.__addControl(
                            ui=pm.floatFieldGrp(label="Environment Samples", numberOfFields = 1),
                            attrName="envSamples")

                with pm.frameLayout(label="Environment", collapsable=True, collapse=False):
                    with pm.columnLayout("appleseedColumnLayout", adjustableColumn=True, width=columnWidth):
                        with pm.rowLayout("appleseedRowLayout", nc=3):
                            pm.text("Environment Light")
                            ui = pm.optionMenu()
                            pm.menuItem(label='<none>')

                            self.__uis["envLight"] = ui
                            # todo: add change callback here...
                            #self.__scriptJobs["envLight"] = mc.scriptJob(
                            #    attributeChange=["appleseedRenderGlobals.envLight", changeCallback])

                        self.__addControl(
                            ui=pm.checkBoxGrp(label="Background Emits Light"),
                            attrName="bgLight")

                with pm.frameLayout(label="System", collapsable=True, collapse=False):
                    with pm.columnLayout("appleseedColumnLayout", adjustableColumn=True, width=columnWidth):
                        self.__addControl(
                            ui=pm.intFieldGrp(label="Threads", numberOfFields = 1),
                            attrName="threads")

        pm.setUITemplate("renderGlobalsTemplate", popTemplate=True)
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(
            parentForm,
            edit=True,
            attachForm=[
                ("appleseedScrollLayout", "top", 0),
                ("appleseedScrollLayout", "bottom", 0),
                ("appleseedScrollLayout", "left", 0),
                ("appleseedScrollLayout", "right", 0)])

        logger.debug("Created appleseed render global main tab")

        # Update the newly created tab.
        self.update()

    def update(self):
        assert(mc.objExists("appleseedRenderGlobals"))


g_appleseedMainTab = AppleseedRenderGlobalsMainTab()

def createRenderTabsMelProcedures():
    mel.eval('''
        global proc appleseedCurrentRendererChanged()
        {
            python("from appleseedMaya.renderGlobals import currentRendererChanged");
            python("currentRendererChanged()");
        }
        '''
    )
    mel.eval('''
        global proc appleseedUpdateCommonTabProcedure()
        {
            updateMayaSoftwareCommonGlobalsTab();

            python("from appleseedMaya.renderGlobals import postUpdateCommonTab");
            python("postUpdateCommonTab()");
        }
        '''
    )
    mel.eval('''
        global proc appleseedCreateAppleseedTabProcedure()
        {
            python("from appleseedMaya.renderGlobals import g_appleseedMainTab");
            python("g_appleseedMainTab.create()");
        }
        '''
    )
    mel.eval('''
        global proc appleseedUpdateAppleseedTabProcedure()
        {
            python("from appleseedMaya.renderGlobals import g_appleseedMainTab");
            python("g_appleseedMainTab.update()");
        }
        '''
    )
