
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

def createAppleseedTab():
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
                    pm.connectControl(
                        pm.intFieldGrp(label="Pixel Samples", numberOfFields = 1),
                        pm.Attribute("appleseedRenderGlobals.samples"),
                        index=2)
                    pm.connectControl(
                        pm.intFieldGrp(label="Render Passes", numberOfFields = 1),
                        pm.Attribute("appleseedRenderGlobals.passes"),
                        index=2)
                    pm.connectControl(
                        pm.intFieldGrp(label="Tile Size", numberOfFields = 1),
                        pm.Attribute("appleseedRenderGlobals.tileSize"),
                        index=2)

            with pm.frameLayout(label="Lighting", collapsable=True, collapse=False):
                with pm.columnLayout("appleseedColumnLayout", adjustableColumn=True, width=columnWidth):
                    pm.connectControl(
                        pm.checkBoxGrp(label="Global Illumination"),
                        pm.Attribute("appleseedRenderGlobals.gi"),
                        index=2)
                    pm.connectControl(
                        pm.checkBoxGrp(label="Caustics"),
                        pm.Attribute("appleseedRenderGlobals.caustics"),
                        index=2)
                    pm.connectControl(
                        pm.intFieldGrp(label="GI Bounces", numberOfFields = 1),
                        pm.Attribute("appleseedRenderGlobals.bounces"),
                        index=2)
                    pm.connectControl(
                        pm.checkBoxGrp(label="Background Emits Light"),
                        pm.Attribute("appleseedRenderGlobals.bgLight"),
                        index=2)

            with pm.frameLayout(label="Environment Light", collapsable=True, collapse=False):
                pm.text(label="Environment light options here...")

            with pm.frameLayout(label="System", collapsable=True, collapse=False):
                with pm.columnLayout("appleseedColumnLayout", adjustableColumn=True, width=columnWidth):
                    pm.connectControl(
                        pm.intFieldGrp(label="Threads", numberOfFields = 1),
                        pm.Attribute("appleseedRenderGlobals.threads"),
                        index=2)

    pm.setUITemplate("renderGlobalsTemplate", popTemplate=True)
    pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
    pm.formLayout(
        parentForm,
        edit=True,
        attachForm=[
            ("appleseedScrollLayout", "top", 0),
            ("appleseedScrollLayout", "bottom", 0),
            ("appleseedScrollLayout", "left", 0),
            ("appleseedScrollLayout", "right", 0)
        ])

    # Update the newly created tab.
    updateAppleseedTab()

def updateAppleseedTab():
    pass

def createRenderTabsMelProcedures():
    mel.eval('''
        global proc appleseedCreateTabProcedure()
        {
            python("from appleseedMaya.renderGlobals import createAppleseedTab");
            python("createAppleseedTab()");
        }
        '''
    )
    mel.eval('''
        global proc appleseedUpdateTabProcedure()
        {
            python("from appleseedMaya.renderGlobals import updateAppleseedTab");
            python("updateAppleseedTab()");
        }
        '''
    )
