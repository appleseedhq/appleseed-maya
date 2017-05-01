
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

# appleseedMaya imports.
from logger import logger


def hyperShadePanelBuildCreateMenuCallback():
    mc.menuItem(label="Appleseed")
    mc.menuItem(divider=True)

def hyperShadePanelBuildCreateSubMenuCallback():
    return "rendernode/appleseed/surface"

def hyperShadePanelPluginChangeCallback(classification, changeType):
    if 'rendernode/appleseed' in classification:
        return 1

    return 0

def createRenderNodeSelectNodeCategoriesCallback(flag, treeLister):
    if flag == "allWithAppleseedUp":
        mc.treeLister(treeLister, edit=True, selectPath="appleseed")

def createRenderNodePluginChangeCallback(classification):
    if 'rendernode/appleseed' in classification:
        return 1

    return 0

def renderNodeClassificationCallback():
    return "rendernode/appleseed"

def createAsRenderNode(nodeType=None, postCommand=None):
    classification = mc.getClassification(nodeType)
    logger.debug(
        "CreateAsRenderNode called: nodeType = {0}, class = {1}, pcmd = {2}".format(
            nodeType,
            classification,
            postCommand
        )
    )

    for cl in classification:
        if "rendernode/appleseed/surface" in cl.lower():
            mat = mc.shadingNode(nodeType, asShader=True)
            shadingGroup = mc.sets(
                renderable=True,
                noSurfaceShader=True,
                empty=True,
                name=mat + "SG"
            )
            mc.connectAttr(mat + ".outColor", shadingGroup + ".surfaceShader")

            if nodeType == 'asGlass':
                mc.setAttr(shadingGroup + ".asDoubleSided", 1)

            logger.debug("Created shading node {0} asShader".format(mat))
        elif "rendernode/appleseed/texture" in cl.lower():
            mat = mc.shadingNode(nodeType, asTexture=True)
            logger.debug("Created shading node {0} asTexture".format(mat))
        else:
            mat = mc.shadingNode(nodeType, asUtility=True)
            logger.debug("Created shading node {0} asUtility".format(mat))

    if postCommand is not None:
        postCommand = postCommand.replace("%node", mat)
        postCommand = postCommand.replace("%type", '\"\"')
        mel.eval(postCommand)

    return ""

def createRenderNodeCallback(postCommand, nodeType):
    #logger.debug("createRenderNodeCallback called!")

    for c in mc.getClassification(nodeType):
        if 'rendernode/appleseed' in c.lower():
            buildNodeCmd = (
                "import appleseedMaya.hyperShadeCallbacks;"
                "appleseedMaya.hyperShadeCallbacks.createAsRenderNode"
                "(nodeType=\\\"{0}\\\", postCommand='{1}')").format(nodeType, postCommand)
            return "string $cmd = \"{0}\"; python($cmd);".format(buildNodeCmd)

def buildRenderNodeTreeListerContentCallback(tl, postCommand, filterString):
    melCmd = 'addToRenderNodeTreeLister("{0}", "{1}", "{2}", "{3}", "{4}", "{5}");'.format(
        tl,
        postCommand,
        "Appleseed/Surface",
        "rendernode/appleseed/surface",
        "-asShader",
        ""
    )
    logger.debug("buildRenderNodeTreeListerContentCallback: mel = %s" % melCmd)
    mel.eval(melCmd)

    melCmd = 'addToRenderNodeTreeLister("{0}", "{1}", "{2}", "{3}", "{4}", "{5}");'.format(
        tl,
        postCommand,
        "Appleseed/2D Textures",
        "rendernode/appleseed/texture/2d",
        "-asTexture",
        ""
    )
    logger.debug("buildRenderNodeTreeListerContentCallback: mel = %s" % melCmd)
    mel.eval(melCmd)

    melCmd = 'addToRenderNodeTreeLister("{0}", "{1}", "{2}", "{3}", "{4}", "{5}");'.format(
        tl,
        postCommand,
        "Appleseed/3D Textures",
        "rendernode/appleseed/texture/3d",
        "-asTexture",
        ""
    )
    logger.debug("buildRenderNodeTreeListerContentCallback: mel = %s" % melCmd)
    mel.eval(melCmd)

    melCmd = 'addToRenderNodeTreeLister("{0}", "{1}", "{2}", "{3}", "{4}", "{5}");'.format(
        tl,
        postCommand,
        "Appleseed/Utilities",
        "rendernode/appleseed/utility",
        "-asUtility",
        ""
    )
    logger.debug("buildRenderNodeTreeListerContentCallback: mel = %s" % melCmd)
    mel.eval(melCmd)

def nodeCanBeUsedAsMaterialCallback(nodeId, nodeOwner):
    logger.debug((
        "nodeCanBeUsedAsMaterialCallback called: "
        "nodeId = {0}, nodeOwner = {1}").format(nodeId, nodeOwner)
    )

    if nodeOwner == 'appleseedMaya':
        return 1

    return 0
