
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

# Standard imports.
import os

# Maya imports.
import maya.cmds as mc

__g_appleseedIconsLocationInit = False
__g_appleseedIconsLocation = None

# It seems Maya cannot find the icons using relative paths,
# even if XBMLANGPATH is set correctly.
# Find our icon in XBMLANGPATH manually.

def appleseedIconsPath():
    global __g_appleseedIconsLocationInit
    global __g_appleseedIconsLocation

    if not __g_appleseedIconsLocationInit:
        __g_appleseedIconsLocationInit = True
        for iconPath in os.environ.get('XBMLANGPATH').split(os.pathsep):
            if os.path.exists(os.path.join(iconPath, "appleseed.png")):
                __g_appleseedIconsLocation = iconPath

    return __g_appleseedIconsLocation

def createLocator(locatorType):
    xformName = mc.createNode("transform", name=locatorType + "1")
    shapeName = xformName.replace(locatorType, locatorType + "Shape")
    return (xformName, mc.createNode(locatorType, name=shapeName, parent=xformName))
