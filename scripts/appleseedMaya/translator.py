
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
import maya.OpenMaya as om
import maya.OpenMayaUI as omui

# appleseedMaya imports.
from logger import logger
from appleseedMaya.renderGlobals import createGlobalNodes


def translatorOptions(parent, action, initialSettings, resultCallback):
    defaults = {
        "activeCamera": "<Current>",
        "exportAnim": False,
        "startFrame": 1,
        "endFrame": 100,
        "stepFrame": 1
    }

    createGlobalNodes()

    if initialSettings:
        logger.debug(
            "Parsing initial translator settings %s" % initialSettings)

        opts = initialSettings.split(";")
        for opt in opts:
            opt = opt.strip()

            if opt == "":
                continue

            name, value = opt.split("=")
            if name in defaults:
                if isinstance(defaults[name], basestring):
                    defaults[name] = value
                elif isinstance(defaults[name], bool):
                    defaults[name] = bool(value)
                elif isinstance(defaults[name], int):
                    defaults[name] = int(value)
                else:
                    logger.warning(
                        "Unhandled param %s in translator options" % name)

    if action == "post":
        mc.setParent(parent)
        mc.setUITemplate("DefaultTemplate", pushTemplate=True)
        mc.columnLayout(adj=True)

        mc.optionMenuGrp(
            "as_exportOpts_activeCamera",
            label="Render camera")
        mc.menuItem(label='<Current>', divider=True)
        for camera in mc.ls(type='camera'):
            if mc.getAttr(camera + '.orthographic'):
                continue
            if not mc.getAttr(camera + ".renderable"):
                continue

            mc.menuItem(label=camera)

        mc.separator(style="single")

        def exportAnimChanged(value):
            mc.intSliderGrp(
                "as_exportOpts_startFrame",
                edit=True,
                enable=value)

            mc.intSliderGrp(
                "as_exportOpts_endFrame",
                edit=True,
                enable=value)

            mc.intSliderGrp(
                "as_exportOpts_stepFrame",
                edit=True,
                enable=value)

        exportAnim = defaults["exportAnim"]
        mc.checkBoxGrp(
            "as_exportOpts_exportAnim",
            numberOfCheckBoxes=1,
            label=" ",
            label1="Animation",
            cc=exportAnimChanged,
            value1=exportAnim)

        mc.intSliderGrp(
            "as_exportOpts_startFrame",
            label="Start:",
            field=True,
            min=1,
            max=1000,
            enable=exportAnim,
            value=defaults["startFrame"])

        mc.intSliderGrp(
            "as_exportOpts_endFrame",
            label="End:",
            field=True,
            min=1,
            max=1000,
            enable=exportAnim,
            value=defaults["endFrame"])

        mc.intSliderGrp(
            "as_exportOpts_stepFrame",
            label="Step:",
            field=True,
            min=1,
            max=100,
            enable=exportAnim,
            value=defaults["stepFrame"])

    elif action == "query":
        options = ""

        value = mc.optionMenuGrp(
            "as_exportOpts_activeCamera", query=True, value=True)

        # Replace current by the active camera.
        if value == "<Current>":
            if om.MGlobal.mayaState() == om.MGlobal.kInteractive:
                camera = om.MDagPath()
                omui.M3dView.active3dView().getCamera(camera)

                if mc.getAttr(camera.partialPathName() + ".renderable"):
                    value = camera.partialPathName()
                else:
                    logger.warning("Active camera not renderable. Ignoring.")
                    value = ""
            else:
                logger.warning("Active camera specified for maya batch.")
                value = ""

        if value:
            options += "activeCamera=" + value + ";"

        exportAnim = mc.checkBoxGrp(
            "as_exportOpts_exportAnim", query=True, value1=True)
        if exportAnim:
            options += "exportAnim=true;"

            value = mc.intSliderGrp(
                "as_exportOpts_startFrame", query=True, value=True)
            options += "startFrame=" + str(value) + ";"

            value = mc.intSliderGrp(
                "as_exportOpts_endFrame", query=True, value=True)
            options += "endFrame=" + str(value) + ";"

            value = mc.intSliderGrp(
                "as_exportOpts_stepFrame", query=True, value=True)
            options += "stepFrame=" + str(value) + ";"

        logger.debug("calling translator callback, options = %s" % options)
        mel.eval('%s "%s"' % (resultCallback, options))


def createTranslatorMelProcedures():
    mel.eval('''
        global proc appleseedTranslatorOpts(string $parent, string $action, string $initialSettings, string $resultCallback)
        {
            python("import appleseedMaya.renderGlobals");
            python("appleseedMaya.renderGlobals.createGlobalNodes()");

            python("import appleseedMaya.translator");
            string $pythonCmd = "appleseedMaya.translator.translatorOptions(";
            $pythonCmd = $pythonCmd + "\\\"" + $parent + "\\\", ";
            $pythonCmd = $pythonCmd + "\\\"" + $action + "\\\", ";
            $pythonCmd = $pythonCmd + "\\\"" + $initialSettings + "\\\", ";
            $pythonCmd = $pythonCmd + "\\\"" + $resultCallback + "\\\")";
            python($pythonCmd);
        }
        '''
             )
