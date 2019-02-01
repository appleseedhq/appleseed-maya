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
        self.beginLayout('Visibility', collapse=1)
        self.addControl('asVisibilityCamera', label='Camera')
        self.addControl('asVisibilityLight', label='Light')
        self.addControl('asVisibilityShadow', label='Shadow')
        self.addControl('asVisibilityDiffuse', label='Diffuse')
        self.addControl('asVisibilitySpecular', label='Specular')
        self.addControl('asVisibilityGlossy', label='Glossy')
        self.endLayout()

    @staticmethod
    def meshAlphaMapCreateNew(node, attr):
        logger.debug("Alpha Map create new: %s.%s" % (node, attr))
        alphaMap = mc.createNode("appleseedAlphaMap")
        mc.connectAttr(alphaMap + ".shape", node + '.' + attr)

    def __meshAlphaCreateNewPyCmd(self, attr):
        (nodeName, attrName) = attr.split('.')
        thisClass = 'appleseedMaya.AETemplates.AEappleseedNodeTemplate'
        return "%s.meshAlphaMapCreateNew('%s', '%s')" % (thisClass, nodeName, attrName)

    def meshAlphaMapNew(self, attr):
        py_cmd = self.__meshAlphaCreateNewPyCmd(attr)
        pm.attrNavigationControlGrp(
            'asAlphaMap',
            label='Alpha Map',
            createNew='python("%s")' % py_cmd,
            at=attr)

    def meshAlphaMapUpdate(self, attr):
        py_cmd = self.__meshAlphaCreateNewPyCmd(attr)
        pm.attrNavigationControlGrp(
            'asAlphaMap',
            edit=True,
            createNew='python("%s")' % py_cmd,
            at=attr)

    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)

        if self.thisNode.type() == 'areaLight':
            self.beginLayout('appleseed', collapse=1)
            self.addControl('asIntensityScale', label='Intensity Scale')
            self.addControl('asExposure', label='Exposure')
            self.addControl('asNormalize', label='Normalize')
            self.__buildVisibilitySection()
            self.endLayout()

        elif self.thisNode.type() == 'bump2d':
            self.beginLayout('appleseed', collapse=1)
            self.addControl('asNormalMapMode', label='Map Mode')
            self.addSeparator()
            self.addControl('asNormalMapFlipR', label='Flip Red Channel')
            self.addControl('asNormalMapFlipG', label='Flip Green Channel')
            self.addControl('asNormalMapSwapRG', label='Swap R/G Channels')
            self.endLayout()

        elif self.thisNode.type() == 'camera':
            self.beginLayout('appleseed', collapse=1)
            self.addControl('asAutofocus', label='Enable Autofocus')
            self.addControl('asHorizontalTarget', label='Horizontal Target')
            self.addControl('asVerticalTarget', label='Vertical Target')
            self.endLayout()

        elif self.thisNode.type() == 'mesh':
            self.beginLayout('appleseed', collapse=1)
            self.__buildVisibilitySection()
            self.callCustom(
                self.meshAlphaMapNew, self.meshAlphaMapUpdate, 'asAlphaMap')
            self.addControl('asMediumPriority', label='Medium Priority')
            self.addControl('asIsPhotonTarget', label='Photon Target')
            self.addControl('asSubsurfaceSet', label='SSS Set')

            self.beginLayout('Export', collapse=1)
            self.addControl('asExportUVs', label='Export UVs')
            self.addControl('asExportNormals', label='Export Normals')
            self.addControl('asSmoothTangents', label='Smooth Tangents')
            self.endLayout()

            self.endLayout()

        elif self.thisNode.type() == 'shadingEngine':
            self.beginLayout('appleseed', collapse=1)
            self.addControl('asDoubleSided', label='Double Sided')
            self.addControl('asShadingSamples', label='Shading Samples')
            self.endLayout()


def appleseedAETemplateCallback(nodeName):
    AEappleseedNodeTemplate(nodeName)
