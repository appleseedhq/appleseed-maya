
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
import pymel.core as pm
import maya.mel as mel

# appleseedMaya imports.
from logger import logger


def hyperShadePanelBuildCreateMenuCallback():
    pm.menuItem(label="Appleseed")
    pm.menuItem(divider=True)

def hyperShadePanelBuildCreateSubMenuCallback():
    return "shader/surface"

def buildRenderNodeTreeListerContentCallback(tl, postCommand, filterString):
    melCmd = 'addToRenderNodeTreeLister("{0}", "{1}", "{2}", "{3}", "{4}", "{5}");'.format(
        tl,
        postCommand,
        "Appleseed/Surface",
        "appleseed/surface",
        "-asShader",
        ""
    )
    logger.debug("buildRenderNodeTreeListerContentCallback: mel = %s" % melCmd)
    mel.eval(melCmd)

    melCmd = 'addToRenderNodeTreeLister("{0}", "{1}", "{2}", "{3}", "{4}", "{5}");'.format(
        tl,
        postCommand,
        "Appleseed/Textures",
        "appleseed/texture",
        "-asUtility",
        ""
    )
    logger.debug("buildRenderNodeTreeListerContentCallback: mel = %s" % melCmd)
    mel.eval(melCmd)

def createRenderNode(nodeType=None, postCommand=None):
    nodeClass = None
    for cl in pm.getClassification(nodeType):
        if "appleseed/surface" in cl.lower():
            nodeClass = "shader"
        if "appleseed/texture" in cl.lower():
            nodeClass = "texture"

    if nodeClass == "shader":
        mat = pm.shadingNode(nodeType, asShader=True)
        shadingGroup = pm.sets(renderable=True, noSurfaceShader=True, empty=True, name="{0}SG".format(mat))
        mat.outColor >> shadingGroup.surfaceShader
    else:
        mat = pm.shadingNode(nodeType, asTexture=True)

    if postCommand is not None:
        postCommand = postCommand.replace("%node", str(mat))
        postCommand = postCommand.replace("%type", '\"\"')
        pm.mel.eval(postCommand)
    return ""

def createRenderNodeCallback(postCommand, nodeType):
    #logger.debug("createRenderNodeCallback called!")

    for c in pm.getClassification(nodeType):
        if 'appleseed' in c.lower():
            buildNodeCmd = "import appleseedMaya.hyperShadeCallbacks; appleseedMaya.hyperShadeCallbacks.createRenderNode(nodeType=\\\"{0}\\\", postCommand='{1}')".format(nodeType, postCommand)
            buildNodeCmd = "string $cmd = \"{0}\"; python($cmd);".format(buildNodeCmd)
            return buildNodeCmd

def connectNodeToNodeOverrideCallback(srcNode, destNode):
    return 1
