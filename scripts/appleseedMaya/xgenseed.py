
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

# XGen imports.
import xgenm as xg

# Maya imports.
import maya.cmds as mc

# appleseedMaya imports.
import xgenseedutil
import xgenseedui


def appleseedExportFrame(self, frame, objFilename):
    '''Export a single appleseed archive frame.'''

    mc.file(
        objFilename + ".appleseedz",
        force=True,
        options="activeCamera=perspShape;",
        typ="appleseedz",
        es=True,
        pr=True,
        de=False
    )

    self.log("XGen appleseed export, filename = " + objFilename + ".appleseedz")


def appleseedExport(self, objs, filename, lod, materialNS):
    '''Export appleseed archives'''

    filename = self.nestFilenameInDirectory(filename, "appleseed")

    lastProgress = self.progress
    self.splitProgress(len(objs))

    self.log("appleseedExport " + filename + lod)

    # Force units to centimeters when exporting.
    prevUnits = mc.currentUnit(query=True, linear=True, fullName=True)
    mc.currentUnit(linear="centimeter")

    prevTime = mc.currentTime(query=True)

    for obj in objs:
        objFilename = filename + "_" + obj.replace("|", "_") + lod
        mc.select(obj, r=True)

        filenames = []
        # Choose to export single file or a sequence.
        frameToken = ""
        if self.startFrame != self.endFrame:
            frameToken = ".${FRAME}"

            dummyFrameFile = open(objFilename + frameToken + ".appleseed", "wt")
            dummyFrameFile.write("STARTFRAME=%4.4d\nENDFRAME=%4.4d\n" % (int(self.startFrame), int(self.endFrame)))
            dummyFrameFile.close()

            for curFrame in range(int(self.startFrame), int(self.endFrame) + 1):
                appleseedExportFrame(self, curFrame, objFilename + ".%4.4d" % int(curFrame))
        else:
            appleseedExportFrame(self, self.startFrame, objFilename)

        if self.curFiles != None:
            materials = self.getSGsFromObj(obj)
            if materials and len(materials) > 0:
                appleseedFilename = objFilename + frameToken + ".appleseedz"
                appleseedExportAppendFile(self, appleseedFilename, materialNS + materials[0], obj, lod)
        self.incProgress()

    mc.currentUnit(linear=prevUnits)
    mc.currentTime(prevTime)

    self.progress = lastProgress


def appleseedExportAppendFile(self, appleseedFilename, material, obj, lod):
    lodList = self.tweakLodAppend(self.curFiles, lod)
    for l in lodList:
        self.addArchiveFile("appleseed", appleseedFilename, material, "", l, 3)


def xgseedArchiveExportInit(selfid):
    '''Export Init callback. Must fill in some arrays on self.'''

    self = xgenseedutil.castSelf(selfid)
    self.batch_plugins.append("appleseedMaya")


def xgseedArchiveExportInfo(selfid):
    '''Export Info callback. Must fill in some arrays on self.'''

    self = xgenseedutil.castSelf(selfid)
    self.archiveDirs.append("appleseed")
    self.archiveLODBeforeExt.append(".${FRAME}.appleseed")
    self.archiveLODBeforeExt.append(".appleseed")
    self.archiveLODBeforeExt.append(".${FRAME}.appleseedz")
    self.archiveLODBeforeExt.append(".appleseedz")


def xgseedArchiveExport(selfid):
    '''
    Main archive export callback.
    Arguments are passed in self.invokeArgs
    '''

    self = xgenseedutil.castSelf(selfid)
    appleseedExport(
        self=self,
        objs=self.invokeArgs[0],
        filename=self.invokeArgs[1],
        lod=self.invokeArgs[2],
        materialNS=self.invokeArgs[3]
    )
