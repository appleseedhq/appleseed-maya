
#
# This source file is part of appleseed.
# Visit http://appleseedhq.net/ for additional information and resources.
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
import types

# XGen imports.
from xgenm import XgExternalAPI as xgapi
import xgenm as xg
from xgenm.ui.widgets import *
from xgenm.ui.util.xgUtil import *
from xgenm.ui.tabs.xgRendererTab import *
from xgenm.ui.util.xgProgressBar import setProgressInfo

# appleseedMaya imports.
from logger import logger
import xgenseedutil


def addMethod(self, method):
    self.__dict__[method.__name__] = types.MethodType(
        method, self, xg.ui.tabs.RendermanRendererTabUI)

g_tabUIinitCalled = False

# RenderAPIRendererTabUIInit callback
# Called at the end of RenderAPIRendererTab.__init__()


def xgseedUI(selfid):
    global g_tabUIinitCalled
    g_tabUIinitCalled = True

    logger.debug("xgenseed xgseedUI called")

    self = xgenseedutil.castSelf(selfid)

    # Extend the RenderAPIRendererTab instance with some of our methods
    addMethod(self, xgseedRefresh)

    expand = ExpandUI('appleseed Settings')
    self.appleseed_expand_settings = expand
    self.layout().addWidget(expand)

    # todo: create widgets here...
    # ...

    # Register the appleseed renderer in the method combo box
    self.addRenderer("appleseed")

# RenderAPIRendererTabUIRefresh callback
# Called at the end of RenderAPIRendererTab.refresh()


def xgseedRefresh(selfid):
    global g_tabUIinitCalled
    if g_tabUIinitCalled == False:
        xgseedUI(selfid)

    self = xgenseedutil.castSelf(selfid)
    vis = self.renderer.currentText() == "appleseed"
    self.appleseed_expand_settings.setVisible(vis)

    # todo: update the UI here...
    # ...

# Callback after description creation to switch to the appleseed render


def xgseedOnCreateDescription(param):
    logger.debug("xgenseed xgseedOnCreateDescription called")

    params = str(param).split(',')
    if len(params) == 2:
        xg.setAttr(
            "renderer", "appleseed", params[1], params[0], "RendermanRenderer")
