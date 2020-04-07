
#
# This source file is part of appleseed.
# Visit https://appleseedhq.net/ for additional information and resources.
#
# This software is released under the MIT license.
#
# Copyright (c) 2016-2019 Esteban Tovagliari, The appleseedhq Organization
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


# Find XGen headers and libraries.
#
#  This module defines
# XGEN_INCLUDE_DIRS - where to find XGEN includes.
# XGEN_LIBRARIES    - List of libraries when using XGEN.
# XGEN_FOUND        - True if XGEN found.


if(APPLE)
  find_path(MAYA_BASE_DIR ../../devkit/include/maya/MFn.h PATH
    ENV MAYA_LOCATION
    "/Applications/Autodesk/maya2020/Maya.app/Contents"
    "/Applications/Autodesk/maya2019/Maya.app/Contents"
    "/Applications/Autodesk/maya2018/Maya.app/Contents"
  )
endif(APPLE)

if(UNIX)
  find_path(MAYA_BASE_DIR include/maya/MFn.h
    PATH
      ENV MAYA_LOCATION
      "/usr/autodesk/maya2020"
      "/usr/autodesk/maya2019"
  )
endif(UNIX)

if(WIN32)
  find_path(MAYA_BASE_DIR include/maya/MFn.h
    PATH
      ENV MAYA_LOCATION
        "C:/Program Files/Autodesk/Maya2020"
        "C:/Program Files/Autodesk/Maya2019"
        "C:/Program Files/Autodesk/Maya2018"
  )
endif(WIN32)

# Look for the includes.
find_path(XGEN_INCLUDE_DIR XGen/XgConfig.h
    HINTS ${MAYA_BASE_DIR}
    PATHS
      ENV_MAYA_LOCATION
      ${MAYA_BASE_DIR}
    PATH_SUFFIXES
    ../../plug-ins/xgen/include
    plug-ins/xgen/include
  DOC "Maya Xgen headers location"
)

# Look for the library.
find_library(XGEN_LIBRARY AdskXGen
    HINTS ${MAYA_BASE_DIR}
    PATHS
      ENV_MAYA_LOCATIOn
      ${MAYA_BASE_DIR}
    PATH_SUFFIXES
    ../../plug-ins/xgen/lib
    plug-ins/xgen/lib
  DOC "Maya Xgen libraries location"
)

# handle the QUIETLY and REQUIRED arguments and set XGEN_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    XGEN DEFAULT_MSG
    XGEN_LIBRARY
    XGEN_INCLUDE_DIR)

# Copy the results to the output variables.
if(XGEN_FOUND)
    set(XGEN_LIBRARIES ${XGEN_LIBRARY})
    set(XGEN_INCLUDE_DIRS ${XGEN_INCLUDE_DIR})
else()
    set(XGEN_LIBRARIES)
    set(XGEN_INCLUDE_DIRS)
endif()

mark_as_advanced(XGEN_LIBRARY XGEN_INCLUDE_DIR)
