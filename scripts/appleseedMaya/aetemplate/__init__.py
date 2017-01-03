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

import maya.cmds as mc
import pymel.core as pm

# appleseedMaya imports.
from appleseedMaya.logger import logger


class AEappleseedNodeTemplate(pm.ui.AETemplate):
    def __init__(self, nodeName):
        super(AEappleseedNodeTemplate, self).__init__(nodeName)
        self.buildBody(nodeName)
        logger.debug('Built custom appleseed AETemplate.')

    def __buildVisibilitySection(self):
        self.beginLayout('Visibility' ,collapse=1)
        self.addControl('as_visibility_camera'  , label='Camera')
        self.addControl('as_visibility_light'   , label='Light')
        self.addControl('as_visibility_shadow'  , label='Shadow')
        self.addControl('as_visibility_diffuse' , label='Diffuse')
        self.addControl('as_visibility_specular', label='Specular')
        self.addControl('as_visibility_glossy'  , label='Glossy')
        self.endLayout()

    def asSurfaceShaderNew(self, attr):
        mc.attrNavigationControlGrp("asSurfaceShader", label="Surface Shader", attribute=attr)

    def asSurfaceShaderUpdate(self, attr):
        mc.attrNavigationControlGrp("asSurfaceShader", edit=True, attribute=attr)

    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)

        if self.thisNode.type() == 'areaLight':
            self.beginLayout('Appleseed' ,collapse=1)
            self.__buildVisibilitySection()
            self.endLayout()

        if self.thisNode.type() == 'camera':
            self.beginLayout('Appleseed' ,collapse=1)
            self.endLayout()

        if self.thisNode.type() == 'mesh':
            self.beginLayout('Appleseed' ,collapse=1)
            self.__buildVisibilitySection()
            self.addControl('as_medium_priority', label='Medium Priority')
            self.endLayout()

        if self.thisNode.type() == 'shadingEngine':
            self.beginLayout('Appleseed' ,collapse=1)
            self.callCustom(self.asSurfaceShaderNew, self.asSurfaceShaderUpdate, "asSurfaceShader")
            self.endLayout()

def appleseedAETemplateCallback(nodeName):
    AEappleseedNodeTemplate(nodeName)
