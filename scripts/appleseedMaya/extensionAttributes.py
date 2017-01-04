
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

# appleseedMaya imports.
from logger import logger

def __addVisibilityFlagsAttributes(nodeType):
    mc.addExtension(
        nodeType=nodeType,
        longName='as_visibility_camera',
        attributeType='bool',
        defaultValue=True,
        category='appleseedMaya')
    mc.addExtension(
        nodeType=nodeType,
        longName='as_visibility_light',
        attributeType='bool',
        defaultValue=True,
        category='appleseedMaya')
    mc.addExtension(
        nodeType=nodeType,
        longName='as_visibility_shadow',
        attributeType='bool',
        defaultValue=True,
        category='appleseedMaya')
    mc.addExtension(
        nodeType=nodeType,
        longName='as_visibility_diffuse',
        attributeType='bool',
        defaultValue=True,
        category='appleseedMaya')
    mc.addExtension(
        nodeType=nodeType,
        longName='as_visibility_specular',
        attributeType='bool',
        defaultValue=True,
        category='appleseedMaya')
    mc.addExtension(
        nodeType=nodeType,
        longName='as_visibility_glossy',
        attributeType='bool',
        defaultValue=True,
        category='appleseedMaya')

def __addShapeAttributes(nodeType):
    pass


def addExtensionAttributes():
    # todo: implement this.

    # Camera extension attributes.
    # ...
    logger.debug('Added camera extension attributes.')

    # Light extension attributes.
    # ...
    logger.debug('Added light extension attributes.')

    # Area light extension attributes.
    __addVisibilityFlagsAttributes('areaLight')
    logger.debug('Added areaLight extension attributes.')

    # Mesh extension attributes.
    __addVisibilityFlagsAttributes('mesh')
    __addShapeAttributes('mesh')
    mc.addExtension(
        nodeType='mesh',
        longName='as_medium_priority',
        attributeType='byte',
        defaultValue=0,
        minValue=0,
        category='appleseedMaya')
    logger.debug('Added mesh extension attributes.')

    # Shading engine extension attributes
    logger.debug('Added shadingEngine extension attributes.')
