
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

# Standard imports.
import os

# Maya imports.
import maya.cmds as mc
import maya.mel as mel

# appleseedMaya imports.
from logger import logger
from util import createLocator


def showAbout():
    if mc.window('appleseedAboutDialog', query=True, exists=True):
        mc.deleteUI('appleseedAboutDialog')

    window = mc.window('appleseedAboutDialog', title='About appleseed-maya')

    mc.columnLayout(rs=20, columnOffset=['both', 22], width=200)

    # Add some empty space. Is there a better way to do this?
    mc.text(label='')

    mc.image(image='appleseed-logo-256.png')

    mc.text(
        label='Plugin version: ' + mc.pluginInfo("appleseedMaya", q=True, v=True),
        font='boldLabelFont',
        align='center')

    mc.text(
        label='Copyright (c) 2018 The appleseedhq Organization',
        font='boldLabelFont',
        align='center')

    mc.text(
        label='This software is released under the MIT license.',
        font='boldLabelFont',
        align='center')

    # Add some empty space. Is there a better way to do this?
    mc.text(label='')

    mc.setParent('..')
    mc.showWindow(window)

__g_appleseedMenu = None


def createSkyDomeLight():
    (xform, shape) = createLocator('appleseedSkyDomeLight')
    # Add the locator to the light set.
    mc.connectAttr(
        xform + '.instObjGroups',
        'defaultLightSet.dagSetMembers',
        nextAvailable=True)


def createPhysicalLight():
    (xform, shape) = createLocator('appleseedPhysicalSkyLight')
    # Add the locator to the light set.
    mc.connectAttr(
        xform + '.instObjGroups',
        'defaultLightSet.dagSetMembers',
        nextAvailable=True)


def createMenu():
    logger.debug("creating appleseed menu.")

    global __g_appleseedMenu
    deleteMenu()

    gMainWindow = mel.eval('$temp1=$gMainWindow')
    __g_appleseedMenu = mc.menu(
        'appleseedMenu', parent=gMainWindow, label='appleseed', tearOff=True)

    mc.menuItem(
        'appleseedLightMenu',
        subMenu=True,
        label='Lights',
        to=True,
        parent='appleseedMenu')
    mc.menuItem(
        label='Create Sky Dome',
        parent='appleseedLightMenu',
        command='import appleseedMaya.menu\nappleseedMaya.menu.createSkyDomeLight()')
    mc.menuItem(
        label='Create Physical Sky',
        parent='appleseedLightMenu',
        command='import appleseedMaya.menu\nappleseedMaya.menu.createPhysicalLight()')

    mc.menuItem(divider=True, parent='appleseedMenu')

    mc.menuItem(
        label='About',
        parent='appleseedMenu',
        command='import appleseedMaya.menu\nappleseedMaya.menu.showAbout()')


def deleteMenu():
    global __g_appleseedMenu

    try:
        mc.deleteUI(__g_appleseedMenu)
        logger.debug("deleted appleseed menu.")
    except:
        pass
