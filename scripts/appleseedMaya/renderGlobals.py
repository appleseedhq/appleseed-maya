
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
import maya.cmds as mc
import maya.mel as mel
import pymel.core as pm
import maya.OpenMaya as om

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
            "Output",
            "appleseedCreateAppleseedOutputTabProcedure",
            "appleseedUpdateAppleseedOutputTabProcedure"
        )
    )

g_nodeAddedCallbackID = None
g_nodeRemovedCallbackID = None
g_environmentLightsList = []

APPLESEED_ENVIRONMENT_LIGHTS = [
    "appleseedSkyDomeLight",
    "appleseedPhysicalSkyLight"]


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
    assert g_nodeAddedCallbackID == None
    g_nodeAddedCallbackID = om.MDGMessage.addNodeAddedCallback(__nodeAdded)

    global g_nodeRemovedCallbackID
    assert g_nodeRemovedCallbackID == None
    g_nodeRemovedCallbackID = om.MDGMessage.addNodeRemovedCallback(
        __nodeRemoved)

    mc.scriptJob(
        attributeChange=[
            "defaultRenderGlobals.currentRenderer",
            "import appleseedMaya.renderGlobals; appleseedMaya.renderGlobals.currentRendererChanged()"
        ]
    )


def removeRenderGlobalsScriptJobs():
    global g_nodeAddedCallbackID
    assert g_nodeAddedCallbackID != None
    om.MMessage.removeCallback(g_nodeAddedCallbackID)
    g_nodeAddedCallbackID = None

    global g_nodeRemovedCallbackID
    assert g_nodeRemovedCallbackID != None
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
    elif newFormat == 1:  # PNG
        mc.setAttr("defaultRenderGlobals.imageFormat", 32)
        mc.setAttr("defaultRenderGlobals.imfkey", "png", type="string")
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

    def __limitBouncesChanged(self, value):
        self._uis["bounces"].setEnable(value)
        self._uis["specularBounces"].setEnable(value)
        self._uis["glossyBounces"].setEnable(value)
        self._uis["diffuseBounces"].setEnable(value)

    def __enableMaxRayIntensityChanged(self, value):
        self._uis["maxRayIntensity"].setEnable(value)

    def __limitPhotonTracingBouncesChanged(self, value):
        self._uis["photonTracingBounces"].setEnable(value)

    def __limitRadianceEstimationBouncesChanged(self, value):
        self._uis["radianceEstimationBounces"].setEnable(value)

    def __enableMaxRayIntensitySPPMChanged(self, value):
        self._uis["maxRayIntensitySPPM"].setEnable(value)

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
                    with pm.columnLayout("appleseedColumnLayout", adjustableColumn=False, width=columnWidth):
                        self._addControl(
                            ui=pm.intSliderGrp(
                                label="Render Passes", field=True, value=1, cw=(3,160), minValue=1, 
                                fieldMinValue=1, maxValue=100, fieldMaxValue=1000000),
                            attrName="passes")

                        self._addControl(
                            ui=pm.checkBoxGrp(
                                label="Adaptive Sampling", changeCommand=self.__adaptiveSamplerChanged),
                            attrName="adaptiveSampling")

                        adaptiveSampling = mc.getAttr("appleseedRenderGlobals.adaptiveSampling")

                        self._addControl(
                            ui=pm.intSliderGrp(
                                label="Min Samples", field=True, value=16, cw=(3,160), minValue=0, 
                                fieldMinValue=0, maxValue=256, fieldMaxValue=1000000, enable=adaptiveSampling),
                            attrName="minPixelSamples")
                        self._addControl(
                            ui=pm.intSliderGrp(
                                label="Max Samples", field=True, value=256, cw=(3,160), minValue=16, 
                                fieldMinValue=0, maxValue=1024, fieldMaxValue=1000000),
                            attrName="samples")
                        self._addControl(
                            ui=pm.intSliderGrp(
                                label="Batch Sample Size", field=True, value=16, cw=(3,160), minValue=1, 
                                fieldMinValue=1, maxValue=128, fieldMaxValue=1000000, enable=adaptiveSampling),
                            attrName="batchSampleSize")
                        self._addControl(
                            ui=pm.floatSliderGrp(
                                label="Noise Threshold", field=True, value=0.1, step=0.02, precision=4, cw=(3,160), minValue=0.0001, 
                                fieldMinValue=0.0, maxValue=2.0, fieldMaxValue=10000.0, enable=adaptiveSampling),
                            attrName="sampleNoiseThreshold")

                        self._addControl(
                            ui=pm.attrEnumOptionMenuGrp(
                                label="Pixel Filter",
                                enumeratedItem=self._getAttributeMenuItems("pixelFilter")),
                            attrName="pixelFilter")
                        self._addControl(
                            ui=pm.floatSliderGrp(
                                label="Pixel Filter Size", field=True, value=1.5, sliderStep=0.5, precision=1, cw=(3,160), minValue=0.5, 
                                fieldMinValue=0.5, maxValue=4.0, fieldMaxValue=20.0),
                            attrName="pixelFilterSize")
                        self._addControl(
                            ui=pm.intSliderGrp(
                                label="Tile Size", field=True, value=64, cw=(3,160), minValue=8, 
                                fieldMinValue=1, maxValue=1024, fieldMaxValue=65536),
                            attrName="tileSize")

                with pm.frameLayout(label="Lighting", collapsable=True, collapse=False):
                    with pm.columnLayout("appleseedColumnLayout", adjustableColumn=True, width=columnWidth):
                        self._addControl(
                            ui=pm.attrEnumOptionMenuGrp(
                                label="Lighting Engine",
                                enumeratedItem=self._getAttributeMenuItems("lightingEngine")),
                            attrName="lightingEngine")
                        self._addControl(
                            ui=pm.attrEnumOptionMenuGrp(
                                label="Light Sampler",
                                enumeratedItem=self._getAttributeMenuItems("lightSamplingAlgorithm")),
                            attrName="lightSamplingAlgorithm")
                        self._addControl(
                            ui=pm.checkBoxGrp(
                                label="Importance Sampling"),
                            attrName="lightImportanceSampling")

                with pm.frameLayout(label="Path Tracing", collapsable=True, collapse=False):
                    with pm.columnLayout("appleseedColumnLayout", adjustableColumn=False, width=columnWidth):
                        self._addControl(
                            ui=pm.checkBoxGrp(label="Caustics"),
                            attrName="caustics")
                        self._addControl(
                            ui=pm.checkBoxGrp(
                                label="Direct Lighting"),
                            attrName="enableDirectLighting")
                        self._addControl(
                            ui=pm.checkBoxGrp(
                                label="Image-Based Lighting"),
                            attrName="enableIBL")
                        self._addControl(
                            ui=pm.checkBoxGrp(
                                label="Limit Bounces", changeCommand=self.__limitBouncesChanged),
                            attrName="limitBounces")
                        limitBounces = mc.getAttr(
                            "appleseedRenderGlobals.limitBounces")
                        self._addControl(
                            ui=pm.intSliderGrp(
                                label="Global Bounces", field=True, value=8, cw=(3,160), minValue=0, 
                                fieldMinValue=0, maxValue=30, fieldMaxValue=100, enable=limitBounces),
                            attrName="bounces")
                        self._addControl(
                            ui=pm.intSliderGrp(
                                label="Diffuse Bounces", field=True, value=3, cw=(3,160), minValue=0, 
                                fieldMinValue=0, maxValue=30, fieldMaxValue=100, enable=limitBounces),
                            attrName="diffuseBounces")
                        self._addControl(
                            ui=pm.intSliderGrp(
                                label="Glossy Bounces", field=True, value=8, cw=(3,160), minValue=0, 
                                fieldMinValue=0, maxValue=30, fieldMaxValue=100, enable=limitBounces),
                            attrName="glossyBounces")
                        self._addControl(
                            ui=pm.intSliderGrp(
                                label="Specular Bounces", field=True, value=8, cw=(3,160), minValue=0, 
                                fieldMinValue=0, maxValue=30, fieldMaxValue=100, enable=limitBounces),
                            attrName="specularBounces")
                        self._addControl(
                            ui=pm.floatSliderGrp(
                                label="Light Samples", field=True, value=1.0, step=1.0, precision=0, cw=(3,160), minValue=0.0, 
                                fieldMinValue=0.0, maxValue=20.0, fieldMaxValue=1000000.0),
                            attrName="lightSamples")
                        self._addControl(
                            ui=pm.floatSliderGrp(
                                label="Environment Samples", field=True, value=1.0, step=1.0, precision=0, cw=(3,160), minValue=0.0, 
                                fieldMinValue=0.0, maxValue=20.0, fieldMaxValue=1000000.0),
                            attrName="envSamples")
                        self._addControl(
                            ui=pm.floatSliderGrp(
                                label="Low Light Threshold",field=True, value=0.0, step=0.01, precision=2, cw=(3,160), minValue=0.0, 
                                fieldMinValue=0.0, maxValue=1.0, fieldMaxValue=1000.0),
                            attrName="lowLightThreshold")
                        self._addControl(
                            ui=pm.checkBoxGrp(label="Clamp Roughness"),
                            attrName="clampRoughness")
                        self._addControl(
                            ui=pm.checkBoxGrp(
                                label="Clamp Ray Intensity", changeCommand=self.__enableMaxRayIntensityChanged),
                            attrName="enableMaxRayIntensity")
                        enableMaxRayIntensity = mc.getAttr(
                                "appleseedRenderGlobals.enableMaxRayIntensity")
                        self._addControl(
                            ui=pm.floatSliderGrp(
                                label="Max Ray Intensity", field=True, value=1.0, step=0.1, precision=1, cw=(3,160), minValue=0.0, 
                                fieldMinValue=0.0, maxValue=2.0, fieldMaxValue=100.0, enable=enableMaxRayIntensity),
                            attrName="maxRayIntensity")

                with pm.frameLayout(label="Stochastic Progressive Photon Mapping", collapsable=True, collapse=False):
                    with pm.columnLayout("appleseedColumnLayout", adjustableColumn=False, width=columnWidth):
                        self._addControl(
                            ui=pm.attrEnumOptionMenuGrp(
                                label="Photon Type",
                                enumeratedItem=self._getAttributeMenuItems("photonType")),
                            attrName="photonType")
                        self._addControl(
                            ui=pm.attrEnumOptionMenuGrp(
                                label="Direct Lighting",
                                enumeratedItem=self._getAttributeMenuItems("SPPMLightingMode")),
                            attrName="SPPMLightingMode")
                        self._addControl(
                            ui=pm.checkBoxGrp(label="Caustics"),
                            attrName="SPPMCaustics")
                        self._addControl(
                            ui=pm.checkBoxGrp(label="Image Based Lighting"),
                            attrName="SPPMEnableIBL")
                        self._addControl(
                            ui=pm.checkBoxGrp(
                                label="Limit PT Bounces", changeCommand=self.__limitPhotonTracingBouncesChanged),
                            attrName="limitPhotonTracingBounces")
                        limitPhotonTracingBounces = mc.getAttr(
                                "appleseedRenderGlobals.limitPhotonTracingBounces")
                        self._addControl(
                            ui=pm.intFieldGrp(
                                label="Max Bounces", columnWidth=(3,120), columnAlign=(3,'left'), 
                                extraLabel ='Photon Tracing', numberOfFields=1, enable=limitPhotonTracingBounces),
                            attrName="photonTracingBounces")
                        self._addControl(
                            ui=pm.intSliderGrp(
                                label="RR Start Bounce", field=True, value=6, cw=(3,160), minValue=1, 
                                fieldMinValue=0, maxValue=30, fieldMaxValue=100),
                            attrName="photonTracingRRMinPathLength")
                        self._addControl(
                            ui=pm.intSliderGrp(
                                label="Light Photons", field=True, value=1000000, cw=(3,160), minValue=100000, 
                                fieldMinValue=0, maxValue=10000000, fieldMaxValue=100000000),
                            attrName="photonTracingLightPhotons")
                        self._addControl(
                            ui=pm.intSliderGrp(
                                label="Environment Photons", field=True, value=1000000, cw=(3,160), minValue=100000, 
                                fieldMinValue=0, maxValue=10000000, fieldMaxValue=100000000),
                            attrName="photonTracingEnvPhotons")
                        self._addControl(
                            ui=pm.checkBoxGrp(
                                label="Limit RE Bounces", changeCommand=self.__limitRadianceEstimationBouncesChanged),
                            attrName="limitRadianceEstimationBounces")
                        limitRadianceEstimationBounces = mc.getAttr(
                                "appleseedRenderGlobals.limitRadianceEstimationBounces")
                        self._addControl(
                            ui=pm.intFieldGrp(
                                label="Max Bounces",  columnWidth=(3,120), columnAlign=(3,'left'), 
                                extraLabel ='Radiance Estimation', numberOfFields=1, enable=limitRadianceEstimationBounces),
                            attrName="radianceEstimationBounces")
                        self._addControl(
                            ui=pm.intSliderGrp(
                                label="RR Start Bounce", field=True, value=6, cw=(3,160), minValue=1, 
                                fieldMinValue=0, maxValue=30, fieldMaxValue=100),
                            attrName="radianceEstimationRRMinPathLength")
                        self._addControl(
                            ui=pm.floatSliderGrp(
                                label="Initial Search Radius", field=True, value=0.1, step=0.05, precision=2, cw=(3,160), minValue=0.01, 
                                fieldMinValue=0.001, maxValue=0.5, fieldMaxValue=100.0),
                            attrName="radianceEstimationInitialRadius")
                        self._addControl(
                            ui=pm.intSliderGrp(
                                label="Max Photons", field=True, value=100, cw=(3,160), minValue=8, 
                                fieldMinValue=8, maxValue=500, fieldMaxValue=1000000000),
                            attrName="radianceEstimationMaxPhotons")
                        self._addControl(
                            ui=pm.floatSliderGrp(
                                label="Alpha", field=True, value=0.7, step=0.05, precision=2, cw=(3,160), minValue=0.0, 
                                fieldMinValue=0.0, maxValue=1.0, fieldMaxValue=1.0),
                            attrName="radianceEstimationAlpha")
                        self._addControl(
                            ui=pm.checkBoxGrp(
                                label="Clamp Ray Intensity", changeCommand=self.__enableMaxRayIntensitySPPMChanged),
                            attrName="enableMaxRayIntensitySPPM")
                        enableMaxRayIntensitySPPM = mc.getAttr(
                                "appleseedRenderGlobals.enableMaxRayIntensitySPPM")
                        self._addControl(
                            ui=pm.floatSliderGrp(
                                label="Max Ray Intensity", field=True, value=1.0, step=0.1, precision=1, cw=(3,160), minValue=0.0, 
                                fieldMinValue=0.0, maxValue=2.0, fieldMaxValue=10000.0, enable=enableMaxRayIntensitySPPM),
                            attrName="maxRayIntensitySPPM")

                with pm.frameLayout(label="Scene", collapsable=True, collapse=False):
                    with pm.columnLayout("appleseedColumnLayout", adjustableColumn=False, width=columnWidth):
                        self._addControl(
                            ui=pm.attrEnumOptionMenuGrp(
                                label="Override Shaders",
                                enumeratedItem=self._getAttributeMenuItems("diagnostics")),
                            attrName="diagnostics")

                        self._addControl(
                            ui=pm.floatSliderGrp(
                                label="Scene Scale", field=True, value=1.0, sliderStep=0.1, precision=2, cw=(3,160), minValue=0.01, 
                                fieldMinValue=1.0e-6, maxValue=100, fieldMaxValue=1.0e+6),
                            attrName="sceneScale")

                        with pm.rowLayout("appleseedRowLayout", nc=3):
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

                        self._addControl(
                            ui=pm.checkBoxGrp(label="Environment Visible"),
                            attrName="bgLight")

                with pm.frameLayout(label="Motion Blur", collapsable=True, collapse=True):
                    with pm.columnLayout("appleseedColumnLayout", adjustableColumn=False, width=columnWidth):
                        self._addControl(
                            ui=pm.checkBoxGrp(
                                label="Motion Blur", changeCommand=self.__motionBlurChanged),
                            attrName="motionBlur")

                        enableMotionBlur = mc.getAttr(
                            "appleseedRenderGlobals.motionBlur")
                        self._addControl(
                            ui=pm.intSliderGrp(
                                label="Camera Samples", field=True, value=2, cw=(3,160), minValue=2, 
                                fieldMinValue=2, maxValue=30, fieldMaxValue=1000, enable=enableMotionBlur),
                            attrName="mbCameraSamples")
                        self._addControl(
                            ui=pm.intSliderGrp(
                                label="Transformation Samples",  field=True, value=2, cw=(3,160), minValue=2, 
                                fieldMinValue=2, maxValue=30, fieldMaxValue=1000, enable=enableMotionBlur),
                            attrName="mbTransformSamples")
                        self._addControl(
                            ui=pm.intSliderGrp(
                                label="Deformation Samples", field=True, value=2, cw=(3,160), minValue=2, 
                                fieldMinValue=2, maxValue=30, fieldMaxValue=1000, enable=enableMotionBlur),
                            attrName="mbDeformSamples")
                        self._addControl(
                            ui=pm.floatSliderGrp(
                                label="Shutter Open", field=True, value=-0.25, sliderStep=0.05, precision=2, cw=(3,160), minValue=-1.0, 
                                fieldMinValue=-1.0, maxValue=0.0, fieldMaxValue=0.0, enable=enableMotionBlur),
                            attrName="shutterOpen")
                        self._addControl(
                            ui=pm.floatSliderGrp(
                                label="Shutter Close", field=True, value=0.25, sliderStep=0.05, precision=2, cw=(3,160), minValue=0.0, 
                                fieldMinValue=0.0, maxValue=1.0, fieldMaxValue=1.0, enable=enableMotionBlur),
                            attrName="shutterClose")

                with pm.frameLayout(label="System", collapsable=True, collapse=False):
                    with pm.columnLayout("appleseedColumnLayout", adjustableColumn=True, width=columnWidth):
                        self._addControl(
                            ui=pm.intFieldGrp(
                                label="Threads", numberOfFields=1),
                            attrName="threads")
                        self._addControl(
                            ui=pm.intFieldGrp(
                                label="Texture Cache Size (MB)", numberOfFields=1),
                            attrName="maxTexCacheSize")

                with pm.frameLayout(label="Experimental", collapsable=True, collapse=False):
                    with pm.columnLayout("appleseedColumnLayout", adjustableColumn=True, width=columnWidth):
                        self._addControl(
                            ui=pm.checkBoxGrp(label="Use Embree"),
                            attrName="useEmbree")

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


class AppleseedRenderGlobalsOutputTab(AppleseedRenderGlobalsTab):

    def __prefilterChanged(self, value):
        self._uis["spikeThreshold"].setEnable(value)

    def __chooseLogFilename(self):
        logger.debug("choose log filename called!")
        path = pm.fileDialog2(fileMode=0)

        if path:
            mc.setAttr("appleseedRenderGlobals.logFilename", path, type="string")

    def create(self):
        # Create default render globals node if needed.
        createGlobalNodes()

        parentForm = pm.setParent(query=True)
        pm.setUITemplate("renderGlobalsTemplate", pushTemplate=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)

        columnWidth = 400

        with pm.scrollLayout("outputScrollLayout", horizontalScrollBarThickness=0):
            with pm.columnLayout("outputColumnLayout", adjustableColumn=True, width=columnWidth):
                with pm.frameLayout(label="AOVs", collapsable=True, collapse=False):
                    with pm.columnLayout("outputColumnLayout", adjustableColumn=True, width=columnWidth):
                        self._addControl(ui=pm.checkBoxGrp(label="Diffuse"), attrName="diffuseAOV")
                        self._addControl(ui=pm.checkBoxGrp(label="Glossy"), attrName="glossyAOV")
                        self._addControl(ui=pm.checkBoxGrp(label="Emission"), attrName="emissionAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Direct Diffuse"), attrName="directDiffuseAOV")
                        self._addControl(ui=pm.checkBoxGrp(label="Indirect Diffuse"), attrName="indirectDiffuseAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Direct Glossy"), attrName="directGlossyAOV")
                        self._addControl(ui=pm.checkBoxGrp(label="Indirect Glossy"), attrName="indirectGlossyAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Albedo"), attrName="albedoAOV")
                        self._addControl(ui=pm.checkBoxGrp(label="Normal"), attrName="normalAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="Invalid Samples"), attrName="invalidSamplesAOV")
                        self._addControl(ui=pm.checkBoxGrp(label="Pixel Sample Count"), attrName="pixelSampleCountAOV")
                        self._addControl(ui=pm.checkBoxGrp(label="Pixel Time"), attrName="pixelTimeAOV")
                        self._addControl(ui=pm.checkBoxGrp(label="Pixel Variation"), attrName="pixelVariationAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="UV"), attrName="uvAOV")
                        self._addControl(ui=pm.checkBoxGrp(label="Depth"), attrName="depthAOV")
                        self._addControl(ui=pm.checkBoxGrp(label="Position"), attrName="positionAOV")

                        self._addControl(ui=pm.checkBoxGrp(label="NPR Shading"), attrName="nprShadingAOV")
                        self._addControl(ui=pm.checkBoxGrp(label="NPR Contours"), attrName="nprContourAOV")

                with pm.frameLayout(label="Denoiser", collapsable=True, collapse=True):
                    with pm.columnLayout("outputColumnLayout", adjustableColumn=True, width=columnWidth):
                        self._addControl(
                            ui=pm.attrEnumOptionMenuGrp(
                                label="Denoiser",
                                enumeratedItem=self._getAttributeMenuItems("denoiser")),
                            attrName="denoiser")

                        self._addControl(
                            ui=pm.checkBoxGrp(
                                label="Skip Already Denoised"),
                            attrName="skipDenoised")

                        self._addControl(
                            ui=pm.checkBoxGrp(
                                label="Random Pixel Order"),
                            attrName="randomPixelOrder")

                        enablePrefilter = mc.getAttr(
                            "appleseedRenderGlobals.prefilterSpikes")
                        self._addControl(
                            ui=pm.checkBoxGrp(
                                label="Prefilter Spikes",
                                changeCommand=self.__prefilterChanged),
                            attrName="prefilterSpikes")
                        self._addControl(
                            ui=pm.floatFieldGrp(
                                label="Spike Thereshold", numberOfFields=1, enable=enablePrefilter),
                            attrName="spikeThreshold")

                        self._addControl(
                            ui=pm.floatFieldGrp(
                                label="Patch Distance", numberOfFields=1),
                            attrName="patchDistance")
                        self._addControl(
                            ui=pm.intFieldGrp(
                                label="Denoise Scales", numberOfFields=1),
                            attrName="denoiseScales")

                with pm.frameLayout(label="Render Stamp", collapsable=True, collapse=True):
                    with pm.columnLayout("outputColumnLayout", adjustableColumn=True, width=columnWidth):
                        self._addControl(ui=pm.checkBoxGrp(label="Enable"), attrName="renderStamp")
                        self._addControl(
                            ui=pm.textFieldGrp(
                                label='Render Stamp'),
                            attrName="renderStampString")

                with pm.frameLayout(label="Logging", collapsable=True, collapse=True):
                    with pm.columnLayout("outputColumnLayout", adjustableColumn=True, width=columnWidth):
                        self._addControl(
                            ui=pm.attrEnumOptionMenuGrp(
                                label="Log Level",
                                enumeratedItem=self._getAttributeMenuItems("logLevel")),
                            attrName="logLevel")

                        self._addControl(
                            ui=pm.textFieldButtonGrp(
                                label='Log Filename',
                                buttonLabel='...',
                                buttonCommand=self.__chooseLogFilename),
                            attrName="logFilename")

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
