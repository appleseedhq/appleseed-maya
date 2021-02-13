
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

global changesunPositioningSystem

def changesunPositioningSystem(nodeName):
        sunPositioningSystem = pm.getAttr(nodeName + ".sunPositioningSystem")
        if sunPositioningSystem == 0:
            pm.setAttr(nodeName + ".sunTheta", lock=False)
            pm.setAttr(nodeName + ".sunPhi", lock=False)

            pm.setAttr(nodeName + ".hour", lock=True)
            pm.setAttr(nodeName + ".minute", lock=True)
            pm.setAttr(nodeName + ".second", lock=True)
            pm.setAttr(nodeName + ".month", lock=True)
            pm.setAttr(nodeName + ".day", lock=True)
            pm.setAttr(nodeName + ".year", lock=True)
            pm.setAttr(nodeName + ".timezone", lock=True)
            pm.setAttr(nodeName + ".north", lock=True)
            pm.setAttr(nodeName + ".latitude", lock=True)
            pm.setAttr(nodeName + ".longitude", lock=True)
        else:
            pm.setAttr(nodeName + ".sunTheta", lock=True)
            pm.setAttr(nodeName + ".sunPhi", lock=True)

            pm.setAttr(nodeName + ".hour", lock=False)
            pm.setAttr(nodeName + ".minute", lock=False)
            pm.setAttr(nodeName + ".second", lock=False)
            pm.setAttr(nodeName + ".month", lock=False)
            pm.setAttr(nodeName + ".day", lock=False)
            pm.setAttr(nodeName + ".year", lock=False)
            pm.setAttr(nodeName + ".timezone", lock=False)
            pm.setAttr(nodeName + ".north", lock=False)
            pm.setAttr(nodeName + ".latitude", lock=False)
            pm.setAttr(nodeName + ".longitude", lock=False)



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
        self.addControl('sunPositioningSystem', changeCommand=changesunPositioningSystem)

        self.beginLayout('Sun Theta/Sun Phi', collapse=0)
        self.addControl('sunTheta')
        self.addControl('sunPhi')
        self.endLayout()

        self.beginLayout('Time and Location', collapse=0)
        self.addControl('hour')
        self.addControl('minute')
        self.addControl('second')
        self.addControl('month')
        self.addControl('day')
        self.addControl('year')
        self.addControl('timezone')
        self.addControl('north')
        self.addControl('latitude')
        self.addControl('longitude')
        self.endLayout()
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
