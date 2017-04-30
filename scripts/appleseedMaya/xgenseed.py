
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

# XGen imports.
import xgenm as xg

# appleseedMaya imports.
from logger import logger
import xgenseedutil
import xgenseedui


def appleseedExport(self, objs, filename, lod, materialNS):
    pass

# Export Init callback. Must fill in some arrays on self
def xgseedArchiveExportInit(selfid):
    logger.debug("xgenseed archive export init.")

    self = xgenseedutil.castSelf(selfid)
    self.batch_plugins.append("appleseedMaya")

# Export Info callback. Must fill in some arrays on self
def xgseedArchiveExportInfo(selfid):
    logger.debug("xgenseed archive export info.")

    self = xgenseedutil.castSelf(selfid)
    self.archiveDirs.append("appleseed")
    self.archiveLODBeforeExt.append(".${FRAME}.appleseed")
    self.archiveLODBeforeExt.append(".appleseed")

# Main Export callback
# Arguments are passed in self.invokeArgs
def xgseedArchiveExport(selfid):
    logger.debug("xgenseed archive export")

    self = xgenseedutil.castSelf(selfid)
    appleseedExport(
        self=self,
        objs=self.invokeArgs[0],
        filename=self.invokeArgs[1],
        lod=self.invokeArgs[2],
        materialNS=self.invokeArgs[3]
    )
