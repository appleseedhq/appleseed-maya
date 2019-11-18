
#
# This source file is part of appleseed.
# Visit https://appleseedhq.net/ for additional information and resources.
#
# This software is released under the MIT license.
#
# Copyright (c) 2016-2019 Esteban Tovagliari, The appleseedhq Organization
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
import maya.OpenMaya as om
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


def createRenderTabsMelProcedures():
    pm.mel.source("createMayaSoftwareCommonGlobalsTab.mel")

    mel.eval('''
        global proc appleseedUpdateCommonTabProcedure()
        {
            updateMayaSoftwareCommonGlobalsTab();

            python("import appleseedMaya.renderGlobals");
            python("appleseedMaya.renderGlobals.postUpdateCommonTab()");
        }
        '''
             )
    mel.eval('''
        global proc appleseedCreateAppleseedMainTabProcedure()
        {
            python("import appleseedMaya.renderGlobals");
            python("appleseedMaya.renderGlobals.g_appleseedMainTab.create()");
        }
        '''
             )
    mel.eval('''
        global proc appleseedUpdateAppleseedMainTabProcedure()
        {
            python("import appleseedMaya.renderGlobals");
            python("appleseedMaya.renderGlobals.g_appleseedMainTab.update()");
        }
        '''
             )
    mel.eval('''
        global proc appleseedCreateAppleseedLightingTabProcedure()
        {
            python("import appleseedMaya.renderGlobals");
            python("appleseedMaya.renderGlobals.g_appleseedLightingTab.create()");
        }
        '''
             )
    mel.eval('''
        global proc appleseedUpdateAppleseedLightingTabProcedure()
        {
            python("import appleseedMaya.renderGlobals");
            python("appleseedMaya.renderGlobals.g_appleseedLightingTab.update()");
        }
        '''
             )
    mel.eval('''
        global proc appleseedCreateAppleseedOutputTabProcedure()
        {
            python("import appleseedMaya.renderGlobals");
            python("appleseedMaya.renderGlobals.g_appleseedOutputTab.create()");
        }
        '''
             )
    mel.eval('''
        global proc appleseedUpdateAppleseedOutputTabProcedure()
        {
            python("import appleseedMaya.renderGlobals");
            python("appleseedMaya.renderGlobals.g_appleseedOutputTab.update()");
        }
        '''
             )
    mel.eval('''
        global proc appleseedCreateAppleseedSystemTabProcedure()
        {
            python("import appleseedMaya.renderGlobals");
            python("appleseedMaya.renderGlobals.g_appleseedSystemTab.create()");
        }
        '''
             )
    mel.eval('''
        global proc appleseedUpdateAppleseedSystemTabProcedure()
        {
            python("import appleseedMaya.renderGlobals");
            python("appleseedMaya.renderGlobals.g_appleseedSystemTab.update()");
        }
        '''
             )


def renderSettingsBuiltCallback(renderer):
    logger.debug("appleseedRenderSettingsBuilt called!")
    pm.renderer(
        "appleseed",
        edit=True,
        addGlobalsTab=(
            "Common",
            "createMayaSoftwareCommonGlobalsTab",
            "appleseedUpdateCommonTabProcedure"
        )
    )
    pm.renderer(
        "appleseed",
        edit=True,
        addGlobalsTab=(
            "appleseed",
            "appleseedCreateAppleseedMainTabProcedure",
            "appleseedUpdateAppleseedMainTabProcedure"
        )
    )
    pm.renderer(
        "appleseed",
        edit=True,
        addGlobalsTab=(
            "Lighting",
            "appleseedCreateAppleseedLightingTabProcedure",
            "appleseedUpdateAppleseedLightingTabProcedure"
        )
    )
    pm.renderer(
        "appleseed",
        edit=True,
        addGlobalsTab=(
            "Output",
            "appleseedCreateAppleseedOutputTabProcedure",
            "appleseedUpdateAppleseedOutputTabProcedure"
        )
    )
    pm.renderer(
        "appleseed",
        edit=True,
        addGlobalsTab=(
            "System",
            "appleseedCreateAppleseedSystemTabProcedure",
            "appleseedUpdateAppleseedSystemTabProcedure"
        )
    )


g_nodeAddedCallbackID = None
g_nodeRemovedCallbackID = None
g_environmentLightsList = []


APPLESEED_ENVIRONMENT_LIGHTS = [
    "appleseedSkyDomeLight",
    "appleseedPhysicalSkyLight"
]

g_columnWidth = 400
g_subColumnWidth = g_columnWidth - 20
g_margin = 2


def __nodeAdded(node, data):
    depNodeFn = om.MFnDependencyNode(node)
    nodeType = depNodeFn.typeName()

    if nodeType in APPLESEED_ENVIRONMENT_LIGHTS:
        logger.debug("Added or removed appleseed environment light")

        global g_environmentLightsList
        g_environmentLightsList.append(depNodeFn.name())
        g_appleseedMainTab.updateEnvLightControl()


def __nodeRemoved(node, data):
    depNodeFn = om.MFnDependencyNode(node)
    nodeType = depNodeFn.typeName()

    if nodeType in APPLESEED_ENVIRONMENT_LIGHTS:
        logger.debug("Removed appleseed environment light")

        global g_environmentLightsList
        g_environmentLightsList.remove(depNodeFn.name())
        g_appleseedMainTab.updateEnvLightControl()


def addRenderGlobalsScriptJobs():
    logger.debug("Adding render globals script jobs")

    global g_nodeAddedCallbackID
    assert g_nodeAddedCallbackID is None
    g_nodeAddedCallbackID = om.MDGMessage.addNodeAddedCallback(__nodeAdded)

    global g_nodeRemovedCallbackID
    assert g_nodeRemovedCallbackID is None
    g_nodeRemovedCallbackID = om.MDGMessage.addNodeRemovedCallback(
        __nodeRemoved)

    # This is evalDeferred so it doesn't get
    # called before createMayaSoftwareCommonGlobalsTab
    python_script = "import appleseedMaya.renderGlobals; appleseedMaya.renderGlobals.currentRendererChanged()"
    mc.scriptJob(
        attributeChange=[
            "defaultRenderGlobals.currentRenderer",
             lambda: mc.evalDeferred(python_script, lowestPriority=True),
        ]
    )

    # For fixing the render globals common tab when opening new scene
    # and the default renderer is appleseed
    mc.scriptJob(
        event=[
            'NewSceneOpened',
            lambda: mc.evalDeferred(python_script, lowestPriority=True),
        ]
    )

    # For fixing the render globals common tab on initial startup of maya
    # when the default renderer is appleseed
    mc.evalDeferred(python_script, lowestPriority=True)


def removeRenderGlobalsScriptJobs():
    global g_nodeAddedCallbackID
    assert g_nodeAddedCallbackID is not None
    om.MMessage.removeCallback(g_nodeAddedCallbackID)
    g_nodeAddedCallbackID = None

    global g_nodeRemovedCallbackID
    assert g_nodeRemovedCallbackID is not None
    om.MMessage.removeCallback(g_nodeRemovedCallbackID)
    g_nodeRemovedCallbackID = None

    logger.debug("Removed render globals script jobs")


def imageFormatChanged():
    logger.debug("imageFormatChanged called")

    # Since we only support two file formats atm., we can hardcode things.
    # 32 is the format code for png, 51 is custom image format.
    # We also update the extension attribute (used in the file names preview).
    newFormat = mc.getAttr("appleseedRenderGlobals.imageFormat")

    if newFormat == 0:  # EXR
        mc.setAttr("defaultRenderGlobals.imageFormat", 51)
        mc.setAttr("defaultRenderGlobals.imfkey", "exr", type="string")
        mc.optionMenuGrp("imageMenuMayaSW", edit=True, select=newFormat + 1)
    elif newFormat == 1:  # PNG
        mc.setAttr("defaultRenderGlobals.imageFormat", 32)
        mc.setAttr("defaultRenderGlobals.imfkey", "png", type="string")
        mc.optionMenuGrp("imageMenuMayaSW", edit=True, select=newFormat + 1)
    else:
        raise RuntimeError("Unknown render global image file format")


def currentRendererChanged():
    newRenderer = mel.eval("currentRenderer()")

    logger.debug("currentRendererChanged called, new renderer = %s", newRenderer)

    if newRenderer != "appleseed":
        return

    # Make sure our render globals node exists.
    createGlobalNodes()

    # If the render globals window does not exist, create it.
    if not mc.window("unifiedRenderGlobalsWindow", exists=True):
        mel.eval("unifiedRenderGlobalsWindow")
        if pm.versions.current() >= 2017000:
            mc.workspaceControl("unifiedRenderGlobalsWindow", edit=True, visible=False)
        else:
            mc.window("unifiedRenderGlobalsWindow", edit=True, visible=False)

    # This can happen if currentRendererChanged is called too soon during startup
    # and unifiedRenderGlobalsWindow isn't complete or delayed for some reason.
    # Known to happen if default renderer is appleseed and the scene is opened as
    # a commandline argument. In that case the NewSceneOpened scriptjob will call the
    # currentRendererChanged function again later.
    if not mc.optionMenuGrp('imageMenuMayaSW', q=True, ex=True):
        logger.warn("imageMenuMayaSW does not exists yet")
        return

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
    mc.connectControl(
        "imageMenuMayaSW", "appleseedRenderGlobals.imageFormat", index=1)
    mc.connectControl(
        "imageMenuMayaSW", "appleseedRenderGlobals.imageFormat", index=2)

    # Add a callback when our internal attribute changes.
    # This callback gets the current value from our internal attribute and
    # uses it to update the original image format attribute (closing the circle.)
    mc.scriptJob(
        parent=parent,
        replacePrevious=True,
        attributeChange=[
            "appleseedRenderGlobals.imageFormat",
            "import appleseedMaya.renderGlobals; appleseedMaya.renderGlobals.imageFormatChanged()"]
    )

    # Update the image format controls now.
    imageFormatChanged()


def postUpdateCommonTab():
    imageFormatChanged()


class AppleseedRenderGlobalsTab(object):

    def __init__(self):
        self._uis = {}

    def _addControl(self, ui, attrName, connectIndex=2):
        self._uis[attrName] = ui
        attr = pm.Attribute("appleseedRenderGlobals." + attrName)
        pm.connectControl(ui, attr, index=connectIndex)

    def _addFieldSliderControl(self, attrName, **kwargs):
        attr = pm.Attribute("appleseedRenderGlobals." + attrName)
        self._uis[attrName] = pm.attrFieldSliderGrp(
            attribute=attr,
            **kwargs)

    def _getAttributeMenuItems(self, attrName):
        attr = pm.Attribute("appleseedRenderGlobals." + attrName)
        menuItems = [
            (i, v) for i, v in enumerate(attr.getEnums().keys())
        ]
        return menuItems


class AppleseedRenderGlobalsMainTab(AppleseedRenderGlobalsTab):

    def __adaptiveSamplerChanged(self, value):
        self._uis["minPixelSamples"].setEnable(value)
        self._uis["batchSampleSize"].setEnable(value)
        self._uis["sampleNoiseThreshold"].setEnable(value)
        if value:
            mc.setAttr("appleseedRenderGlobals.samples", 256)
        else:
            mc.setAttr("appleseedRenderGlobals.samples", 32)

    def __motionBlurChanged(self, value):
        self._uis["mbCameraSamples"].setEnable(value)
        self._uis["mbTransformSamples"].setEnable(value)
        self._uis["mbDeformSamples"].setEnable(value)
        self._uis["shutterOpen"].setEnable(value)
        self._uis["shutterClose"].setEnable(value)

    def __environmentLightSelected(self, envLight):
        logger.debug("Environment light selected: %s" % envLight)

        connections = mc.listConnections(
            "appleseedRenderGlobals.envLight",
            plugs=True)
        if connections:
            mc.disconnectAttr(
                connections[0], "appleseedRenderGlobals.envLight")

        if envLight != "<none>":
            mc.connectAttr(
                envLight + ".globalsMessage",
                "appleseedRenderGlobals.envLight")

    def updateEnvLightControl(self):
        if "envLight" in self._uis:
            logger.debug("Updating env lights menu")

            uiName = self._uis["envLight"]

            # Return if the menu does not exist yet.
            if not pm.optionMenu(uiName, exists=True):
                return

            # Remove the callback.
            pm.optionMenu(uiName, edit=True, changeCommand="")

            # Delete the menu items.
            items = pm.optionMenu(uiName, query=True, itemListLong=True)
            for item in items:
                pm.deleteUI(item)

            connections = mc.listConnections("appleseedRenderGlobals.envLight")

            # Rebuild the menu.
            pm.menuItem(parent=uiName, label="<none>")
            for envLight in g_environmentLightsList:
                pm.menuItem(parent=uiName, label=envLight)

            # Update the currently selected item.
            if connections:
                node = connections[0]
                if mc.nodeType(node) == "transform":
                    shapes = mc.listRelatives(node, shapes=True)
                    assert shapes
                    node = shapes[0]
                    pm.optionMenu(uiName, edit=True, value=node)
            else:
                pm.optionMenu(uiName, edit=True, value="<none>")

            # Restore the callback.
            pm.optionMenu(
                uiName, edit=True, changeCommand=self.__environmentLightSelected)

    def __lockSamplingPatternChanged(self, value):
        self._uis["noiseSeed"].setEnable(value)

    def create(self):
        # Create default render globals node if needed.
        createGlobalNodes()

        parentForm = pm.setParent(query=True)
        pm.setUITemplate("renderGlobalsTemplate", pushTemplate=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)

        with pm.scrollLayout("appleseedScrollLayout", horizontalScrollBarThickness=0):
            with pm.columnLayout("appleseedColumnLayout", adjustableColumn=True, width=g_columnWidth):

                with pm.frameLayout("samplingFrameLayout", label="Sampling", collapsable=True, collapse=False):
                    with pm.columnLayout("samplingColumnLayout", adjustableColumn=True, width=g_subColumnWidth,
                                         rowSpacing=2):

                        pm.separator(height=2)

                        self._addFieldSliderControl(
                            label="Render Passes",
                            columnWidth=(3, 160),
                            columnAttach=(1, "right", 4),
                            minValue=1,
                            fieldMinValue=1,
                            maxValue=100,
                            fieldMaxValue=1000000,
                            attrName="passes")

                        pm.separator(height=2)

                        self._addControl(
                            ui=pm.checkBoxGrp(
                                label="Adaptive Sampling",
                                height=18,
                                columnAttach=(1, "right", 4),
                                changeCommand=self.__adaptiveSamplerChanged),
                            attrName="adaptiveSampling")

                        pm.separator(height=2)

                        adaptiveSampling = mc.getAttr("appleseedRenderGlobals.adaptiveSampling")

                        self._addFieldSliderControl(
                            label="Min Samples",
                            columnWidth=(3, 160),
                            columnAttach=(1, "right", 4),
                            minValue=0,
                            fieldMinValue=0,
                            maxValue=256,
                            fieldMaxValue=1000000,
                            enable=adaptiveSampling,
                            attrName="minPixelSamples")

                        self._addFieldSliderControl(
                            label="Max Samples",
                            columnWidth=(3, 160),
                            columnAttach=(1, "right", 4),
                            minValue=16,
                            fieldMinValue=0,
                            maxValue=1024,
                            fieldMaxValue=1000000,
                            attrName="samples")

                        self._addFieldSliderControl(
                            label="Batch Sample Size",
                            columnWidth=(3, 160),
                            columnAttach=(1, "right", 4),
                            minValue=1,
                            fieldMinValue=1,
                            maxValue=128,
                            fieldMaxValue=1000000,
                            enable=adaptiveSampling,
                            attrName="batchSampleSize")

                        self._addFieldSliderControl(
                            label="Noise Threshold",
                            step=0.02,
                            precision=4,
                            columnWidth=(3, 160),
                            columnAttach=(1, "right", 4),
                            minValue=0.0001,
                            fieldMinValue=0.0,
                            maxValue=2.0,
                            fieldMaxValue=25.0,
                            enable=adaptiveSampling,
                            attrName="sampleNoiseThreshold")

                        pm.separator(height=2)

                        self._addControl(
                            ui=pm.attrEnumOptionMenuGrp(
                                label="Pixel Filter",
                                columnAttach=(1, "right", 4),
                                enumeratedItem=self._getAttributeMenuItems("pixelFilter")),
                            attrName="pixelFilter")

                        self._addFieldSliderControl(
                            label="Pixel Filter Size",
                            sliderStep=0.5,
                            precision=1,
                            columnWidth=(3, 160),
                            columnAttach=(1, "right", 4),
                            minValue=0.5,
                            fieldMinValue=0.5,
                            maxValue=4.0,
                            fieldMaxValue=20.0,
                            attrName="pixelFilterSize")

                        self._addFieldSliderControl(
                            label="Tile Size",
                            columnWidth=(3, 160),
                            columnAttach=(1, "right", 4),
                            minValue=8,
                            fieldMinValue=1,
                            maxValue=1024,
                            fieldMaxValue=65536,
                            attrName="tileSize")

                        pm.separator(height=2)

                        self._addControl(
                            ui=pm.checkBoxGrp(
                                label="Lock Sampling Pattern",
                                height=18,
                                columnAttach=(1, "right", 4),
                                changeCommand=self.__lockSamplingPatternChanged),
                            attrName="lockSamplingPattern")

                        lockSamplingPattern = mc.getAttr("appleseedRenderGlobals.lockSamplingPattern")

                        pm.separator(height=2)

                        self._addFieldSliderControl(
                            label="Sampling Pattern Seed",
                            columnWidth=(3, 160),
                            minValue=-65536,
                            fieldMinValue=-2147483648,
                            maxValue=65535,
                            enable=lockSamplingPattern,
                            fieldMaxValue=2147483647,
                            attrName="noiseSeed")

                        pm.separator(height=2)

                with pm.frameLayout("motionBlurFrameLayout", label="Motion Blur", collapsable=True, collapse=True):
                    with pm.columnLayout("motionBlurColumnLayout", adjustableColumn=True, width=g_subColumnWidth,
                                         rowSpacing=2):

                        pm.separator(height=2)

                        self._addControl(
                            ui=pm.checkBoxGrp(
                                label="Motion Blur",
                                height=18,
                                columnAttach=(1, "right", 4),
                                changeCommand=self.__motionBlurChanged),
                            attrName="motionBlur")

                        pm.separator(height=2)

                        enableMotionBlur = mc.getAttr(
                            "appleseedRenderGlobals.motionBlur")

                        self._addFieldSliderControl(
                            label="Camera Samples",
                            columnWidth=(3, 160),
                            columnAttach=(1, "right", 4),
                            minValue=2,
                            fieldMinValue=2,
                            maxValue=30,
                            fieldMaxValue=1000,
                            enable=enableMotionBlur,
                            attrName="mbCameraSamples")

                        self._addFieldSliderControl(
                            label="Transformation Samples",
                            columnWidth=(3, 160),
                            columnAttach=(1, "right", 4),
                            minValue=2,
                            fieldMinValue=2,
                            maxValue=30,
                            fieldMaxValue=1000,
                            enable=enableMotionBlur,
                            attrName="mbTransformSamples")

                        self._addFieldSliderControl(
                            label="Deformation Samples",
                            columnWidth=(3, 160),
                            columnAttach=(1, "right", 4),
                            minValue=2,
                            fieldMinValue=2,
                            maxValue=30,
                            fieldMaxValue=1000,
                            enable=enableMotionBlur,
                            attrName="mbDeformSamples")

                        pm.separator(height=2)

                        self._addFieldSliderControl(
                            label="Shutter Open",
                            sliderStep=0.05,
                            precision=2,
                            columnWidth=(3, 160),
                            columnAttach=(1, "right", 4),
                            minValue=-1.0,
                            fieldMinValue=-1.0,
                            maxValue=0.0,
                            fieldMaxValue=0.0,
                            enable=enableMotionBlur,
                            attrName="shutterOpen")

                        self._addFieldSliderControl(
                            label="Shutter Close",
                            sliderStep=0.05,
                            precision=2,
                            columnWidth=(3, 160),
                            columnAttach=(1, "right", 4),
                            minValue=0.0,
                            fieldMinValue=0.0,
                            maxValue=1.0,
                            fieldMaxValue=1.0,
                            enable=enableMotionBlur,
                            attrName="shutterClose")

                        pm.separator(height=2)

                with pm.frameLayout("sceneFrameLayout", label="Scene", collapsable=True, collapse=False):
                    with pm.columnLayout("sceneColumnLayout", adjustableColumn=True, width=g_subColumnWidth,
                                         rowSpacing=2):

                        pm.separator(height=2)

                        self._addFieldSliderControl(
                            label="Scene Scale",
                            sliderStep=0.1,
                            precision=2,
                            columnWidth=(3, 160),
                            columnAttach=(1, "right", 4),
                            minValue=0.01,
                            fieldMinValue=1.0e-6,
                            maxValue=100,
                            fieldMaxValue=1.0e+6,
                            attrName="sceneScale")

                        pm.separator(height=2)

                        with pm.rowLayout("appleseedRowLayout", numberOfColumns=3, columnAttach=(1, "right", 4)):
                            pm.text("Environment Light")
                            ui = pm.optionMenu(
                                changeCommand=self.__environmentLightSelected)

                            pm.menuItem(label="<none>")
                            for envLight in g_environmentLightsList:
                                pm.menuItem(parent=ui, label=envLight)

                            # Set the currently selected environment light in the menu.
                            connections = mc.listConnections(
                                "appleseedRenderGlobals.envLight")

                            if connections:
                                node = connections[0]
                                if mc.nodeType(node) == "transform":
                                    shapes = mc.listRelatives(node, shapes=True)
                                    assert shapes
                                    node = shapes[0]
                                    pm.optionMenu(ui, edit=True, value=node)
                            else:
                                pm.optionMenu(ui, edit=True, value="<none>")

                            self._uis["envLight"] = ui
                            logger.debug(
                                "Created globals env light menu, name = %s" % ui)

                        pm.separator(height=2)

                        self._addControl(
                            ui=pm.checkBoxGrp(
                                label="Environment Visible",
                                columnAttach=(1, "right", 4),
                                height=18),
                            attrName="bgLight")

                        pm.separator(height=2)

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
        assert mc.objExists("appleseedRenderGlobals")
        # self.updateEnvLightControl()


g_appleseedMainTab = AppleseedRenderGlobalsMainTab()


class AppleseedRenderGlobalsLightingTab(AppleseedRenderGlobalsTab):

    def __limitBouncesChanged(self, value):
        self._uis["bounces"].setEnable(value)
        self._uis["specularBounces"].setEnable(value)
        self._uis["glossyBounces"].setEnable(value)
        self._uis["diffuseBounces"].setEnable(value)

    def __enableMaxRayIntensityChanged(self, value):
        self._uis["maxRayIntensity"].setEnable(value)

    def __enableIBLChanged(self, value):
        self._uis["envSamples"].setEnable(value)

    def __enableDirectLightingChanged(self, value):
        self._uis["lightSamples"].setEnable(value)

    def __limitPhotonTracingBouncesChanged(self, value):
        self._uis["photonTracingBounces"].setEnable(value)

    def __limitRadianceEstimationBouncesChanged(self, value):
        self._uis["radianceEstimationBounces"].setEnable(value)

    def __enableMaxRayIntensitySPPMChanged(self, value):
        self._uis["maxRayIntensitySPPM"].setEnable(value)

    def __enablePhotonTracingEnvPhotonsChanged(self, value):
        self._uis["photonTracingEnvPhotons"].setEnable(value)

    def create(self):
        # Create default render globals node if needed.
        createGlobalNodes()

        parentForm = pm.setParent(query=True)
        pm.setUITemplate("renderGlobalsTemplate", pushTemplate=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)

        with pm.scrollLayout("lightingScrollLayout", horizontalScrollBarThickness=0):
            with pm.columnLayout("lightingColumnLayout", adjustableColumn=True, width=g_columnWidth):

                with pm.frameLayout("lightingFrameLayout", label="Lighting", collapsable=True, collapse=False):
                    with pm.columnLayout("lightingColumnLayout", adjustableColumn=True, width=g_subColumnWidth,
                                         rowSpacing=2):

                        pm.separator(height=2)

                        self._addControl(
                            ui=pm.attrEnumOptionMenuGrp(
                                label="Lighting Engine",
                                enumeratedItem=self._getAttributeMenuItems("lightingEngine"),
                                columnAttach=[(1, "right", 4), (2, "right", 0)],
                                columnWidth=[(1, 120), (2, 240)],
                                height=22),
                            attrName="lightingEngine")

                        pm.separator(height=2)

                        with pm.frameLayout("pathTracingFrameLayout", label="Unidirectional Path Tracing",
                                            collapsable=True, collapse=True):
                            with pm.columnLayout("pathTracingColumnLayout", adjustableColumn=True,
                                                 width=g_subColumnWidth - g_margin, rowSpacing=2):
                                pm.separator(height=2)

                                with pm.rowColumnLayout("pathTracingRowColumnLayout", numberOfRows=2,
                                                        rowOffset=[(1, "top", 2), (2, "both", 2), (3, "both", 2)],
                                                        rowSpacing=(2, 2)):
                                    self._addControl(
                                        ui=pm.checkBoxGrp(
                                            label="Caustics",
                                            columnAttach=(1, "right", 4)),
                                        attrName="caustics")

                                    self._addControl(
                                        ui=pm.checkBoxGrp(
                                            label="Direct Lighting",
                                            columnAttach=(1, "right", 4),
                                            changeCommand=self.__enableDirectLightingChanged),
                                        attrName="enableDirectLighting")

                                    self._addControl(
                                        ui=pm.checkBoxGrp(
                                            label="Image-Based Lighting",
                                            columnAttach=(1, "right", 4),
                                            changeCommand=self.__enableIBLChanged),
                                        attrName="enableIBL")

                                    self._addControl(
                                        ui=pm.checkBoxGrp(
                                            label="Limit Bounces",
                                            columnAttach=(1, "right", 4),
                                            changeCommand=self.__limitBouncesChanged),
                                        attrName="limitBounces")

                                pm.separator(height=2)

                                limitBounces = mc.getAttr(
                                    "appleseedRenderGlobals.limitBounces")

                                enableIBL = mc.getAttr(
                                    "appleseedRenderGlobals.enableIBL")

                                enableDirectLighting = mc.getAttr(
                                    "appleseedRenderGlobals.enableDirectLighting")

                                self._addFieldSliderControl(
                                    label="Global Bounces",
                                    columnWidth=[(1, 115), (2, 40), (3, 200)],
                                    columnAttach=[(1, "right", 4), (2, "right", 2), (3, "right", 8)],
                                    minValue=0,
                                    maxValue=30,
                                    fieldMinValue=0,
                                    fieldMaxValue=100,
                                    attrName="bounces")

                                self._addFieldSliderControl(
                                    label="Diffuse Bounces",
                                    columnWidth=[(1, 115), (2, 40), (3, 200)],
                                    columnAttach=[(1, "right", 4), (2, "right", 2), (3, "right", 8)],
                                    rowAttach=[(1, "both", 4)],
                                    enable=limitBounces,
                                    minValue=0,
                                    maxValue=30,
                                    fieldMinValue=0,
                                    fieldMaxValue=100,
                                    attrName="diffuseBounces")

                                self._addFieldSliderControl(
                                    label="Glossy Bounces",
                                    columnWidth=[(1, 115), (2, 40), (3, 200)],
                                    columnAttach=[(1, "right", 4), (2, "right", 2), (3, "right", 8)],
                                    enable=limitBounces,
                                    minValue=0,
                                    maxValue=30,
                                    fieldMinValue=0,
                                    fieldMaxValue=100,
                                    attrName="glossyBounces")

                                self._addFieldSliderControl(
                                    label="Specular Bounces",
                                    columnWidth=[(1, 115), (2, 40), (3, 200)],
                                    columnAttach=[(1, "right", 4), (2, "right", 2), (3, "right", 8)],
                                    enable=limitBounces,
                                    minValue=0,
                                    maxValue=30,
                                    fieldMinValue=0,
                                    fieldMaxValue=100,
                                    attrName="specularBounces")

                                pm.separator(height=2)

                                """
                                self._addControl(
                                    ui=pm.attrEnumOptionMenuGrp(
                                        label="Light Sampler",
                                        enumeratedItem=self._getAttributeMenuItems("lightSamplingAlgorithm"),
                                        columnAttach=[(1, "right", 2), (2, "right", 0)],
                                        columnWidth=[(1, 120), (2, 240)]),
                                    attrName="lightSamplingAlgorithm")

                                pm.separator(height=2)
                                """

                                with pm.rowColumnLayout("pathTracingLightISRowColumnLayout", numberOfRows=1):
                                    self._addControl(
                                        ui=pm.checkBoxGrp(
                                            label1="Lights Importance Sampling",
                                            height=18,
                                            columnAttach=[(1, "right", 4), (2, "right", 4)],
                                            columnWidth=[(1, 86), (2, 200)]),
                                        attrName="lightImportanceSampling")

                                pm.separator(height=2)

                                self._addFieldSliderControl(
                                    label="Light Samples",
                                    sliderStep=1.0,
                                    fieldStep=0.1,
                                    precision=1,
                                    columnWidth=[(1, 115), (2, 40), (3, 200)],
                                    columnAttach=[(1, "right", 4), (2, "right", 2), (3, "right", 0)],
                                    rowAttach=[(1, "both", 4)],
                                    enable=enableDirectLighting,
                                    minValue=0.0,
                                    maxValue=20.0,
                                    fieldMinValue=0.0,
                                    fieldMaxValue=1000000.0,
                                    annotation="Number of light samples used to estimate direct lighting.",
                                    attrName="lightSamples")

                                self._addFieldSliderControl(
                                    label="IBL Samples",
                                    sliderStep=1.0,
                                    fieldStep=0.1,
                                    precision=1,
                                    columnWidth=[(1, 115), (2, 40), (3, 240)],
                                    columnAttach=[(1, "right", 4), (2, "right", 2), (3, "right", 0)],
                                    enable=enableIBL,
                                    minValue=0.0,
                                    maxValue=20.0,
                                    fieldMinValue=0.0,
                                    fieldMaxValue=1000000.0,
                                    annotation="Number of samples used to estimate environment or image-based lighting.",
                                    attrName="envSamples")

                                self._addFieldSliderControl(
                                    label="Low Light Threshold",
                                    precision=6,
                                    columnWidth=[(1, 115), (2, 60), (3, 190)],
                                    columnAttach=[(1, "right", 4), (2, "right", 2), (3, "right", 0)],
                                    minValue=0.0,
                                    maxValue=1.0,
                                    fieldMinValue=0.0,
                                    fieldMaxValue=1000.0,
                                    annotation="Threshold at which shadow rays are terminated.",
                                    attrName="lowLightThreshold")

                                pm.separator(height=2)

                                enableMaxRayIntensity = True

                                with pm.rowColumnLayout("pathTracingClampingRowColumnLayout", numberOfRows=1):
                                    self._addControl(
                                        ui=pm.checkBoxGrp(
                                            label="Clamp Roughness",
                                            height=18,
                                            columnAttach=(1, "right", 4),
                                            annotation="Clamp roughness on secondary and subsequent rays."),
                                        attrName="clampRoughness")

                                    self._addControl(
                                        ui=pm.checkBoxGrp(
                                            label="Clamp Ray Intensity",
                                            height=18,
                                            columnAttach=(1, "right", 4),
                                            annotation="Clamp secondary and subsequent rays intensity in order to reduce fireflies.",
                                            changeCommand=self.__enableMaxRayIntensityChanged),
                                        attrName="enableMaxRayIntensity")

                                    enableMaxRayIntensity = mc.getAttr("appleseedRenderGlobals.enableMaxRayIntensity")

                                pm.separator(height=2)

                                self._addFieldSliderControl(
                                    label="Max Ray Intensity",
                                    sliderStep=1.0,
                                    fieldStep=0.1,
                                    precision=1,
                                    columnWidth=[(1, 100), (2, 40), (3, 200)],
                                    columnAttach=[(1, "right", 4), (2, "right", 2), (3, "right", 0)],
                                    minValue=0.0,
                                    maxValue=2.0,
                                    fieldMinValue=0.0,
                                    fieldMaxValue=10000.0,
                                    annotation="Maximum ray intensity allowed on secondary and subsequent rays.",
                                    enable=enableMaxRayIntensity,
                                    attrName="maxRayIntensity")

                                pm.separator(height=2)

                        with pm.frameLayout("sppmFrameLayout", label="Stochastic Progressive Photon Mapping",
                                            collapsable=True, collapse=True):
                            with pm.columnLayout("sppmColumnLayout", adjustableColumn=True,
                                                 width=g_subColumnWidth - g_margin,
                                                 rowSpacing=2):

                                pm.separator(height=2)

                                self._addControl(
                                    ui=pm.attrEnumOptionMenuGrp(
                                        label="Photon Type",
                                        enumeratedItem=self._getAttributeMenuItems("photonType"),
                                        columnAttach=(1, "right", 4),
                                        annotation="Type of photons for the SPPM engine: single-wavelength or wavelength-dependent"),
                                    attrName="photonType")

                                self._addControl(
                                    ui=pm.attrEnumOptionMenuGrp(
                                        label="Direct Lighting",
                                        enumeratedItem=self._getAttributeMenuItems("SPPMLightingMode"),
                                        columnAttach=(1, "right", 4),
                                        annotation="Type of direct lighting engine to use: raytraced, photon or none."),
                                    attrName="SPPMLightingMode")

                                pm.separator(height=2)

                                with pm.rowColumnLayout("sppmCheckBoxesRowColumnLayout", numberOfRows=1):
                                    self._addControl(
                                        ui=pm.checkBoxGrp(
                                            label="Caustics",
                                            columnAttach=(1, "right", 4)),
                                        attrName="SPPMCaustics")

                                    self._addControl(
                                        ui=pm.checkBoxGrp(
                                            label="Image Based Lighting",
                                            columnAttach=(1, "right", 5),
                                            changeCommand=self.__enablePhotonTracingEnvPhotonsChanged),
                                        attrName="SPPMEnableIBL")

                                pm.separator(height=2)

                                with pm.frameLayout("sppmPhotonTracingFrameLayout", font="smallBoldLabelFont",
                                                    label="Photon Tracing", collapsable=False, collapse=True):
                                    with pm.columnLayout("sppmPhotonTracingColumnLayout", adjustableColumn=True,
                                                         width=g_subColumnWidth - g_margin, rowSpacing=2):
                                        pm.separator(height=2)

                                        self._addControl(
                                            ui=pm.checkBoxGrp(
                                                label="Limit Bounces",
                                                columnAttach=(1, "right", 4),
                                                annotation="Restrict the number of photon bounces.",
                                                changeCommand=self.__limitPhotonTracingBouncesChanged),
                                            attrName="limitPhotonTracingBounces")

                                        limitPhotonTracingBounces = mc.getAttr(
                                            "appleseedRenderGlobals.limitPhotonTracingBounces")

                                        pm.separator(height=2)

                                        self._addFieldSliderControl(
                                            label="Max Bounces",
                                            columnWidth=[(1, 125), (2, 40), (3, 100)],
                                            columnAttach=[(1, "right", 4), (2, "right", 2), (3, "right", 8)],
                                            minValue=0,
                                            maxValue=30,
                                            fieldMinValue=0,
                                            fieldMaxValue=100,
                                            annotation="Maximum number of photon bounces.",
                                            enable=limitPhotonTracingBounces,
                                            attrName="photonTracingBounces")

                                        self._addFieldSliderControl(
                                            label="RR Starting Bounce",
                                            columnWidth=[(1, 125), (2, 40), (3, 100)],
                                            columnAttach=[(1, "right", 4), (2, "right", 2), (3, "right", 8)],
                                            minValue=0,
                                            maxValue=30,
                                            fieldMinValue=0,
                                            fieldMaxValue=100,
                                            annotation="Discard low contribution paths starting with this bounce.",
                                            attrName="photonTracingRRMinPathLength")

                                        pm.separator(height=2)

                                        self._addFieldSliderControl(
                                            label="Light Photons",
                                            minValue=100000,
                                            maxValue=10000000,
                                            fieldMinValue=0,
                                            fieldMaxValue=100000000,
                                            columnWidth=[(1, 125), (2, 60), (3, 100)],
                                            columnAttach=[(1, "right", 4), (2, "right", 2), (3, "right", 8)],
                                            annotation="Number of light photons per render pass.",
                                            attrName="photonTracingLightPhotons")

                                        SPPMEnableIBL = mc.getAttr(
                                            "appleseedRenderGlobals.SPPMEnableIBL")

                                        self._addFieldSliderControl(
                                            label="IBL Photons",
                                            minValue=100000,
                                            maxValue=10000000,
                                            fieldMinValue=0,
                                            fieldMaxValue=100000000,
                                            columnWidth=[(1, 125), (2, 60), (3, 100)],
                                            columnAttach=[(1, "right", 4), (2, "right", 2), (3, "right", 8)],
                                            enable=SPPMEnableIBL,
                                            annotation="Number of environment photons per render pass.",
                                            attrName="photonTracingEnvPhotons")

                                with pm.frameLayout("sppmRadianceEstimationFrameLayout", font="smallBoldLabelFont",
                                                    label="Radiance Estimation", collapsable=False, collapse=True):
                                    with pm.columnLayout("sppmRadianceEstimationColumnLayout", adjustableColumn=True,
                                                         width=g_subColumnWidth - g_margin, rowSpacing=2):

                                        pm.separator(height=2)

                                        self._addControl(
                                            ui=pm.checkBoxGrp(
                                                label="Limit Bounces",
                                                columnAttach=(1, "right", 4),
                                                annotation="Restrict the number of path bounces.",
                                                changeCommand=self.__limitRadianceEstimationBouncesChanged),
                                            attrName="limitRadianceEstimationBounces")

                                        limitRadianceEstimationBounces = mc.getAttr(
                                            "appleseedRenderGlobals.limitRadianceEstimationBounces"
                                        )

                                        pm.separator(height=2)

                                        self._addFieldSliderControl(
                                            label="Maximum Bounces",
                                            columnWidth=[(1, 125), (2, 40), (3, 100)],
                                            columnAttach=[(1, "right", 4), (2, "right", 2), (3, "right", 8)],
                                            minValue=0,
                                            maxValue=30,
                                            fieldMinValue=0,
                                            fieldMaxValue=100,
                                            enable=limitRadianceEstimationBounces,
                                            annotation="Maximum number of radiance estimation path bounces.",
                                            attrName="radianceEstimationBounces")

                                        self._addFieldSliderControl(
                                            label="RR Starting Bounces",
                                            columnWidth=[(1, 125), (2, 40), (3, 100)],
                                            columnAttach=[(1, "right", 4), (2, "right", 2), (3, "right", 8)],
                                            minValue=0,
                                            maxValue=30,
                                            fieldMinValue=0,
                                            fieldMaxValue=100,
                                            annotation="Discard low contribution paths starting with this bounce.",
                                            attrName="radianceEstimationRRMinPathLength")

                                        pm.separator(height=2)

                                        self._addFieldSliderControl(
                                            label="Initial Search Radius",
                                            sliderStep=1.0,
                                            fieldStep=0.1,
                                            precision=3,
                                            columnWidth=[(1, 125), (2, 60), (3, 100)],
                                            columnAttach=[(1, "right", 4), (2, "right", 2), (3, "right", 0)],
                                            minValue=0.01,
                                            maxValue=0.5,
                                            fieldMinValue=0.001,
                                            fieldMaxValue=100.0,
                                            annotation="Initial photon gathering radius in percent of scene diameter.",
                                            attrName="radianceEstimationInitialRadius")

                                        self._addFieldSliderControl(
                                            label="Maximum Photons",
                                            columnWidth=[(1, 125), (2, 60), (3, 100)],
                                            columnAttach=[(1, "right", 4), (2, "right", 2), (3, "right", 8)],
                                            minValue=8,
                                            maxValue=500,
                                            fieldMinValue=8,
                                            fieldMaxValue=1000000000,
                                            annotation="Maximum number of photons used for radiance estimation.",
                                            attrName="radianceEstimationMaxPhotons")

                                        self._addFieldSliderControl(
                                            label="Alpha",
                                            sliderStep=1.0,
                                            fieldStep=0.05,
                                            precision=3,
                                            columnWidth=[(1, 125), (2, 60), (3, 100)],
                                            columnAttach=[(1, "right", 4), (2, "right", 2), (3, "right", 0)],
                                            minValue=0.0,
                                            maxValue=1.0,
                                            fieldMinValue=0.0,
                                            fieldMaxValue=1.0,
                                            annotation="Evolution rate of photon gathering radius.",
                                            attrName="radianceEstimationAlpha")

                                        pm.separator(height=2)

                                        self._addControl(
                                            ui=pm.checkBoxGrp(
                                                label="Clamp Ray Intensity",
                                                columnAttach=(1, "right", 4),
                                                annotation="Clamp secondary and subsequent rays intensity in order to reduce fireflies.",
                                                changeCommand=self.__enableMaxRayIntensitySPPMChanged),
                                            attrName="enableMaxRayIntensitySPPM")

                                        enableMaxRayIntensitySPPM = mc.getAttr(
                                            "appleseedRenderGlobals.enableMaxRayIntensitySPPM"
                                        )

                                        pm.separator(height=2)

                                        self._addFieldSliderControl(
                                            label="Maximum Ray Intensity",
                                            sliderStep=1.0,
                                            fieldStep=0.1,
                                            precision=1,
                                            columnWidth=[(1, 125), (2, 40), (3, 100)],
                                            columnAttach=[(1, "right", 4), (2, "right", 2), (3, "right", 0)],
                                            minValue=0.0,
                                            maxValue=2.0,
                                            fieldMinValue=0.0,
                                            fieldMaxValue=10000.0,
                                            enable=enableMaxRayIntensitySPPM,
                                            annotation="Maximum Ray Intensity valued allowed on secondary and subsequent rays.",
                                            attrName="maxRayIntensitySPPM")

        pm.setUITemplate("renderGlobalsTemplate", popTemplate=True)
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(
            parentForm,
            edit=True,
            attachForm=[
                ("lightingScrollLayout", "top", 0),
                ("lightingScrollLayout", "bottom", 0),
                ("lightingScrollLayout", "left", 0),
                ("lightingScrollLayout", "right", 0)])

        logger.debug("Created appleseed lighting engines render globals main tab.")

        # Update the newly created tab.
        self.update()

    def update(self):
        assert mc.objExists("appleseedRenderGlobals")


g_appleseedLightingTab = AppleseedRenderGlobalsLightingTab()


class AppleseedRenderGlobalsOutputTab(AppleseedRenderGlobalsTab):

    def __prefilterChanged(self, value):
        self._uis["spikeThreshold"].setEnable(value)

    def __renderStampChanged(self, value):
        self._uis["renderStampString"].setEnable(value)

    def create(self):
        # Create default render globals node if needed.
        createGlobalNodes()

        parentForm = pm.setParent(query=True)
        pm.setUITemplate("renderGlobalsTemplate", pushTemplate=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)

        with pm.scrollLayout("outputScrollLayout", horizontalScrollBarThickness=0):
            with pm.columnLayout("outputColumnLayout", adjustableColumn=True, width=g_columnWidth):

                with pm.frameLayout("outputAOVSframeLayout", label="AOVs", collapsable=True, collapse=False):
                    with pm.rowColumnLayout("outputAOVsColumnLayout", adjustableColumn=True, width=g_columnWidth,
                                            numberOfColumns=2, rowSpacing=(2, 2)):

                        pm.separator(height=2)
                        pm.separator(height=2)

                        self._addControl(ui=pm.checkBoxGrp(label="Diffuse",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="diffuseAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Glossy",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="glossyAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Emission",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="emissionAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Direct Diffuse",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="directDiffuseAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Indirect Diffuse",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="indirectDiffuseAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Direct Glossy",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="directGlossyAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Indirect Glossy",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="indirectGlossyAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Albedo",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="albedoAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Normal",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="normalAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Invalid Samples",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="invalidSamplesAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Pixel Error",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="pixelErrorAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Pixel Sample Count",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="pixelSampleCountAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Pixel Time",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="pixelTimeAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Pixel Variation",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="pixelVariationAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="UV",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="uvAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Depth",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="depthAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Position",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="positionAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Screen Space Velocity",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="velocityAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Cryptomatte Material",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="cryptomatteMaterialAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Cryptomatte Object",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="cryptomatteObjectAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="NPR Shading",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="nprShadingAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="NPR Contours",
                                                           columnAttach=[(1, "right", 1), (2, "both", 8)]),
                                         attrName="nprContourAOV")

                        pm.separator(height=2)
                        pm.separator(height=2)

                with pm.frameLayout("outputDenoiserFrameLayout", label="Denoiser", collapsable=True, collapse=True):
                    with pm.columnLayout("outputDenoiserColumnLayout", adjustableColumn=True, width=g_columnWidth,
                                         rowSpacing=2):

                        pm.separator(height=2)

                        self._addControl(
                            ui=pm.attrEnumOptionMenuGrp(
                                label="Denoiser",
                                columnAttach=(1, "right", 4),
                                enumeratedItem=self._getAttributeMenuItems("denoiser")),
                            attrName="denoiser")

                        pm.separator(height=2)

                        with pm.columnLayout("denoiserCheckboxesRowColumnLayout",
                                             adjustableColumn=True,
                                             width=g_columnWidth,
                                             rowSpacing=2):

                            self._addControl(
                                ui=pm.checkBoxGrp(
                                    label="Skip Already Denoised",
                                    columnAttach=(1, "right", 4)),
                                attrName="skipDenoised")

                            self._addControl(
                                ui=pm.checkBoxGrp(
                                    label="Random Pixel Order",
                                    columnAttach=(1, "right", 4)),
                                attrName="randomPixelOrder")

                            enablePrefilter = mc.getAttr(
                                "appleseedRenderGlobals.prefilterSpikes")

                            self._addControl(
                                ui=pm.checkBoxGrp(
                                    label="Prefilter Spikes",
                                    columnAttach=(1, "right", 4),
                                    changeCommand=self.__prefilterChanged),
                                attrName="prefilterSpikes")

                        pm.separator(height=2)

                        self._addControl(
                            ui=pm.floatFieldGrp(
                                label="Spike Threshold",
                                numberOfFields=1,
                                columnAttach=(1, "right", 4),
                                enable=enablePrefilter),
                            attrName="spikeThreshold")

                        self._addControl(
                            ui=pm.floatFieldGrp(
                                label="Patch Distance",
                                columnAttach=(1, "right", 4),
                                numberOfFields=1),
                            attrName="patchDistance")

                        self._addControl(
                            ui=pm.intFieldGrp(
                                label="Denoise Scales",
                                columnAttach=(1, "right", 4),
                                numberOfFields=1),
                            attrName="denoiseScales")

                with pm.frameLayout("outputRenderStampFrameLayout", label="Render Stamp", collapsable=True,
                                    collapse=True):
                    with pm.columnLayout("outputRenderStampColumnLayout", adjustableColumn=True, width=g_columnWidth,
                                         rowSpacing=2):

                        pm.separator(height=2)

                        enableRenderStamp = mc.getAttr(
                            "appleseedRenderGlobals.renderStamp")

                        self._addControl(
                            ui=pm.checkBoxGrp(
                                label="Enable",
                                height=18,
                                columnAttach=(1, "right", 4),
                                changeCommand=self.__renderStampChanged),
                            attrName="renderStamp")

                        pm.separator(height=2)

                        self._addControl(
                            ui=pm.textFieldGrp(
                                label="Render Stamp",
                                height=22,
                                columnAttach=(1, "right", 4),
                                enable=enableRenderStamp,
                                annotation="Render stamp allows {lib-name|version|cpu-features|config|build-date|build-time}\n{render-time} and {peak-memory}."),
                            attrName="renderStampString")

                        pm.separator(height=2)

        pm.setUITemplate("renderGlobalsTemplate", popTemplate=True)
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(
            parentForm,
            edit=True,
            attachForm=[
                ("outputScrollLayout", "top", 0),
                ("outputScrollLayout", "bottom", 0),
                ("outputScrollLayout", "left", 0),
                ("outputScrollLayout", "right", 0)])

        logger.debug("Created appleseed render global output tab")

        # Update the newly created tab.
        self.update()

    def update(self):
        assert mc.objExists("appleseedRenderGlobals")


g_appleseedOutputTab = AppleseedRenderGlobalsOutputTab()


class AppleseedRenderGlobalsSystemTab(AppleseedRenderGlobalsTab):

    def __chooseLogFilename(self):
        logger.debug("Choose log filename called!")
        path = pm.fileDialog2(filemode=0)

        if path:
            mc.setAttr("appleseedRenderGlobals.logFilename", path, type="string")

    def create(self):
        # Create default render globals node if needed
        createGlobalNodes()

        parentForm = pm.setParent(query=True)
        pm.setUITemplate("renderGlobalsTemplate", pushTemplate=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)

        columnWidth = 400

        with pm.scrollLayout("diagnosticsScrollLayout", horizontalScrollBarThickness=0):
            with pm.columnLayout("diagnosticsColumnLayout", adjustableColumn=True, width=g_columnWidth):

                with pm.frameLayout("overrideShadersFrameLayout", label="Override Shaders", collapsable=True,
                                    collapse=True):
                    with pm.columnLayout("overrideShadersColumnLayout", adjustableColumn=False, width=g_columnWidth):

                        self._addControl(
                            ui=pm.attrEnumOptionMenuGrp(
                                label="Override Shaders",
                                columnAttach=(1, "right", 4),
                                enumeratedItem=self._getAttributeMenuItems("diagnostics")),
                            attrName="diagnostics")

                with pm.frameLayout("loggingFrameLayout", label="Logging", collapsable=True, collapse=True):
                    with pm.columnLayout("LoggingColumnLayout", adjustableColumn=True, width=g_columnWidth):

                        pm.separator(height=2)

                        self._addControl(
                            ui=pm.attrEnumOptionMenuGrp(
                                label="Log Level",
                                height=24,
                                columnAttach=(1, "right", 4),
                                enumeratedItem=self._getAttributeMenuItems("logLevel"),
                                width=200),
                            attrName="logLevel")

                        pm.separator(height=2)

                        self._addControl(
                            ui=pm.textFieldButtonGrp(
                                label="Log Filename",
                                buttonLabel="...",
                                height=22,
                                columnAttach=(1, "right", 4),
                                buttonCommand=self.__chooseLogFilename),
                            attrName="logFilename")

                        pm.separator(height=2)

                with pm.frameLayout("systemFrameLayout", label="System", collapsable=True, collapse=False):
                    with pm.columnLayout("systemColumnLayout", adjustableColumn=True, width=g_columnWidth):

                        pm.separator(height=2)

                        self._addControl(
                            ui=pm.intFieldGrp(
                                label="Threads",
                                columnAttach=(1, "right", 4),
                                numberOfFields=1),
                            attrName="threads")

                        self._addControl(
                            ui=pm.intFieldGrp(
                                label="Texture Cache Size (MB)",
                                columnAttach=(1, "right", 4),
                                numberOfFields=1),
                            attrName="maxTexCacheSize")

                        pm.separator(height=2)

                with pm.frameLayout("experimentalFrameLayout", label="Experimental", collapsable=True, collapse=False):
                    with pm.columnLayout("experimentalColumnLayout", adjustableColumn=True, width=g_columnWidth):

                        self._addControl(
                            ui=pm.checkBoxGrp(
                                label="Use Embree",
                                columnAttach=(1, "right", 4),
                                height=24),
                            attrName="useEmbree")

        logger.debug("Created appleseed render global diagnostics tab.")

        pm.setUITemplate("renderGlobalsTemplate", popTemplate=True)
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(
            parentForm,
            edit=True,
            attachForm=[
                ("diagnosticsScrollLayout", "top", 0),
                ("diagnosticsScrollLayout", "bottom", 0),
                ("diagnosticsScrollLayout", "left", 0),
                ("diagnosticsScrollLayout", "right", 0)])

        # Update the newly created tab.
        self.update()

    def update(self):
        assert mc.objExists("appleseedRenderGlobals")


g_appleseedSystemTab = AppleseedRenderGlobalsSystemTab()
