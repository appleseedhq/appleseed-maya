
#
# This source file is part of appleseed.
# Visit http://appleseedhq.net/ for additional information and resources.
#
# This software is released under the MIT license.
#
# Copyright (c) 2016 Esteban Tovagliari, The appleseedhq Organization
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

    def __updateGIControls(self):
        self.__uis["bounces"].setEnable(mc.getAttr("appleseedRenderGlobals.gi"))

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

                with pm.frameLayout(label="Lighting", collapsable=True, collapse=False):
                    with pm.columnLayout("appleseedColumnLayout", adjustableColumn=True, width=columnWidth):
                        self.__addControl(
                            ui=pm.checkBoxGrp(label="Global Illumination"),
                            attrName="gi",
                            changeCallback=self.__updateGIControls)
                        self.__addControl(
                            ui=pm.checkBoxGrp(label="Caustics"),
                            attrName="caustics")
                        self.__addControl(
                            ui=pm.intFieldGrp(label="GI Bounces", numberOfFields = 1),
                            attrName="bounces")

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
        global proc appleseedCreateTabProcedure()
        {
            python("from appleseedMaya.renderGlobals import g_appleseedMainTab");
            python("g_appleseedMainTab.create()");
        }
        '''
    )
    mel.eval('''
        global proc appleseedUpdateTabProcedure()
        {
            python("from appleseedMaya.renderGlobals import g_appleseedMainTab");
            python("g_appleseedMainTab.update()");
        }
        '''
    )


'''
def environment_select_create(self, attr=None):
    self.environment_select_layout = mc.rowLayout(nc=3)
    mc.text('Environment')
    self.environment_select_option_menu = mc.optionMenu(cc=partial(self.environment_select_set, attr))
    if attr is not None:
        self.environment_select_update(attr)


        def environment_select_update(self, attr):
            if self.environment_select_option_menu is not None:
                items = mc.optionMenu(self.environment_select_option_menu, q=True, ill=True)
                if items is not None:
                    for item in items:
                        mc.deleteUI(item)
                mc.menuItem(label='<none>', p=self.environment_select_option_menu)
                for environment in mc.ls(type='ms_environment') + mc.ls(type='ms_physical_environment'):
                    mc.menuItem(label=environment, p=self.environment_select_option_menu)
                connection = mc.listConnections(attr, sh=True)
                if connection is None:
                    mc.optionMenu(self.environment_select_option_menu, e=True, v='<none>')
                else:
                    mc.optionMenu(self.environment_select_option_menu, e=True, v=connection[0])


        def environment_select_set(self, attr, environment):
            value = mc.optionMenu(self.environment_select_option_menu, q=True, v=True)
            connection = mc.listConnections(attr, sh=True)
            if value == '<none>':
                if connection is not None:
                    mc.disconnectAttr(connection[0] + '.message', attr)
            else:
                if connection is not None:
                    if connection[0] == environment:
                        return
                mc.connectAttr(environment + '.message', attr, f=True)
'''
