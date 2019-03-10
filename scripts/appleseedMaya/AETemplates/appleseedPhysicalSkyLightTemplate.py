
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

# Maya imports.
import pymel.core as pm

# appleseedMaya imports.
from appleseedMaya.logger import logger


class AEappleseedPhysicalSkyLightTemplate(pm.ui.AETemplate):

    def __init__(self, nodeName):
        super(AEappleseedPhysicalSkyLightTemplate, self).__init__(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()

    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)

    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)

    def buildBody(self, nodeName):
        self.beginLayout('Sky Attributes', collapse=0)
        self.addControl('sunTheta')
        self.addControl('sunPhi')
        self.addSeparator()
        self.addControl('turbidity')
        self.addControl('turbidityScale')
        self.addSeparator()
        self.addControl('luminanceScale')
        self.addControl('luminanceGamma')
        self.addControl('saturationScale')
        self.addSeparator()
        self.addControl('horizonShift')
        self.addControl('groundAlbedo')
        self.endLayout()

        self.beginLayout('Sun Attributes', collapse=0)
        self.addControl('sunEnable')
        self.addSeparator()
        self.addControl('sunRadianceScale')
        self.addControl('sunSizeScale')
        self.endLayout()

        self.beginLayout('Display', collapse=1)
        self.addControl('size', label='Size')
        self.endLayout()
