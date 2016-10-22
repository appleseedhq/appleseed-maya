
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

# Maya imports.
import pymel.core as pm
import maya.mel as mel

# appleseed-maya imports.
from logger import logger
from renderer import createRenderMelProcedures
from renderglobals import createRenderTabsMelProcedures
from callbacks import hyperShadePanelBuildCreateMenuCallback
from callbacks import hyperShadePanelBuildCreateSubMenuCallback
from callbacks import buildRenderNodeTreeListerContentCallback


def register():
    logger.info("Registering appleseed renderer.")

    createRenderMelProcedures()
    createRenderTabsMelProcedures()

    # Renderer.
    pm.renderer("appleseed", rendererUIName="appleseed")
    pm.renderer(
        "appleseed",
        edit=True,
        renderProcedure="appleseedRenderProcedure",
        commandRenderProcedure="appleseedRenderProcedure",
        batchRenderProcedure="appleseedBatchRenderProcedure"
        )

    # Globals
    pm.renderer("appleseed", edit=True, addGlobalsNode="defaultRenderGlobals")
    pm.renderer("appleseed", edit=True, addGlobalsNode="defaultResolution")
    pm.renderer("appleseed", edit=True, addGlobalsNode="appleseedRenderGlobals")

    pm.renderer(
        "appleseed",
        edit=True,
        addGlobalsTab=(
            "Common",
            "createMayaSoftwareCommonGlobalsTab",
            "updateMayaSoftwareCommonGlobalsTab"
            )
        )

    pm.renderer(
        "appleseed",
        edit=True,
        addGlobalsTab=(
            "Appleseed",
            "appleseedCreateTabProcedure",
            "appleseedUpdateTabProcedure"
            )
        )

    # Callbacks
    pm.callbacks(
        addCallback=hyperShadePanelBuildCreateMenuCallback,
        hook="hyperShadePanelBuildCreateMenu",
        owner="appleseed"
    )
    pm.callbacks(
        addCallback=hyperShadePanelBuildCreateSubMenuCallback,
        hook="hyperShadePanelBuildCreateSubMenu",
        owner="appleseed"
    )
    pm.callbacks(
        addCallback=buildRenderNodeTreeListerContentCallback,
        hook='buildRenderNodeTreeListerContent',
        owner="appleseed"
    )

def unregister():
    logger.info("Unregistering appleseed renderer.")

    pm.callbacks(clearCallbacks=True, owner="appleseed")

    if pm.renderer("appleseed", q=True, ex=True):
        pm.renderer("appleseed", unregisterRenderer=True)
