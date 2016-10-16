
# - Maya finder module
# This module searches for a valid Maya instalation.
# It searches for Maya's devkit, libraries, executables
# and related paths (scripts)
#
# Variables that will be defined:
# MAYA_FOUND          Defined if a Maya installation has been detected
# MAYA_EXECUTABLE     Path to Maya's executable
# MAYA_<lib>_FOUND    Defined if <lib> has been found
# MAYA_<lib>_LIBRARY  Path to <lib> library
# MAYA_INCLUDE_DIRS   Path to the devkit's include directories
# MAYA_API_VERSION    Maya API version
#
# IMPORTANT: Currently, there's only support for OSX platform and Maya version 2012.

#=============================================================================
# Copyright 2011-2012 Francisco Requena <frarees@gmail.com>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

## add one to this list to match your install if none match

if(APPLE)
  find_path(MAYA_BASE_DIR ../../devkit/include/maya/MFn.h PATH
    ENV MAYA_LOCATION
    "/Applications/Autodesk/maya2016/Maya.app/Contents"
    "/Applications/Autodesk/maya2015/Maya.app/Contents"
    "/Applications/Autodesk/maya2014/Maya.app/Contents"
    "/Applications/Autodesk/maya2013/Maya.app/Contents"
    "/Applications/Autodesk/maya2012.17/Maya.app/Contents"
    "/Applications/Autodesk/maya2012/Maya.app/Contents"
    "/Applications/Autodesk/maya2011/Maya.app/Contents"
    "/Applications/Autodesk/maya2010/Maya.app/Contents"
  )
  find_path(MAYA_LIBRARY_DIR libOpenMaya.dylib
    PATHS
      ENV MAYA_LOCATION
      ${MAYA_BASE_DIR}
    PATH_SUFFIXES
      Maya.app/Contents/MacOS/
    DOC "Maya's libraries path"
  )
endif(APPLE)

if(UNIX)
  find_path(MAYA_BASE_DIR include/maya/MFn.h
    PATH
      ENV MAYA_LOCATION
      "/usr/autodesk/maya2016"
      "/usr/autodesk/maya2015-x64"
      "/usr/autodesk/maya2014-x64"
      "/usr/autodesk/maya2013-x64"
      "/usr/autodesk/maya2012.17-x64"
      "/usr/autodesk/maya2012-x64"
      "/usr/autodesk/maya2011-x64"
      "/usr/autodesk/maya2010-x64"
  )
  find_path(MAYA_LIBRARY_DIR libOpenMaya.so
    PATHS
      ENV MAYA_LOCATION
      ${MAYA_BASE_DIR}
    PATH_SUFFIXES
      lib/
    DOC "Maya's libraries path"
  )
endif(UNIX)

if(WIN32)
  find_path(MAYA_BASE_DIR include/maya/MFn.h
    PATH
      ENV MAYA_LOCATION
        "C:/Program Files/Autodesk/Maya2016"
        "C:/Program Files/Autodesk/Maya2015"
        "C:/Program Files/Autodesk/Maya2014"
        "C:/Program Files/Autodesk/Maya2013"
        "C:/Program Files/Autodesk/Maya2012-x64"
        "C:/Program Files/Autodesk/Maya2012"
        "C:/Program Files (x86)/Autodesk/Maya2012"
        "C:/Autodesk/maya-2012x64"
        "C:/Program Files/Autodesk/Maya2011-x64"
        "C:/Program Files/Autodesk/Maya2011"
        "C:/Program Files (x86)/Autodesk/Maya2011"
        "C:/Autodesk/maya-2011x64"
        "C:/Program Files/Autodesk/Maya2010-x64"
        "C:/Program Files/Autodesk/Maya2010"
        "C:/Program Files (x86)/Autodesk/Maya2010"
        "C:/Autodesk/maya-2010x64"
  )
  find_path(MAYA_LIBRARY_DIR OpenMaya.lib
    PATHS
      ENV MAYA_LOCATION
      ${MAYA_BASE_DIR}
    PATH_SUFFIXES
      lib/
    DOC "Maya's libraries path"
  )
endif(WIN32)

find_path(MAYA_INCLUDE_DIR maya/MFn.h
  PATHS
    ENV MAYA_LOCATION
    ${MAYA_BASE_DIR}
  PATH_SUFFIXES
    ../../devkit/include/
    include/
  DOC "Maya's devkit headers path"
)

find_path(MAYA_LIBRARY_DIR OpenMaya
  PATHS
    ENV MAYA_LOCATION
    ${MAYA_BASE_DIR}
  PATH_SUFFIXES
    ../../devkit/include/
    include/
  DOC "Maya's devkit headers path"
)

list(APPEND MAYA_INCLUDE_DIRS ${MAYA_INCLUDE_DIR})

find_path(MAYA_DEVKIT_INC_DIR GL/glext.h
  PATHS
    ENV MAYA_LOCATION
    ${MAYA_BASE_DIR}
  PATH_SUFFIXES
    ../../devkit/plug-ins/
    /devkit/plug-ins/
  DOC "Maya's devkit headers path"
)

list(APPEND MAYA_INCLUDE_DIRS ${MAYA_DEVKIT_INC_DIR})

foreach(MAYA_LIB
  OpenMaya
  OpenMayaAnim
  OpenMayaFX
  OpenMayaRender
  OpenMayaUI
  Image
  Foundation
  IMFbase
  tbb
  cg
  cgGL
)
  find_library(MAYA_${MAYA_LIB}_LIBRARY ${MAYA_LIB}
    PATHS
      ENV MAYA_LOCATION
      ${MAYA_BASE_DIR}
    PATH_SUFFIXES
      MacOS/
      lib/
    DOC "Maya's ${MAYA_LIB} library path"
  )

  list(APPEND ${MAYA_LIBRARIES} MAYA_${MAYA_LIB}_LIBRARY)
endforeach(MAYA_LIB)

if(MAYA_INCLUDE_DIRS AND EXISTS "${MAYA_INCLUDE_DIR}/maya/MTypes.h")
    file(STRINGS ${MAYA_INCLUDE_DIR}/maya/MTypes.h TMP REGEX "#define MAYA_API_VERSION.*$")
    string(REGEX MATCHALL "[0-9]+" MAYA_API_VERSION ${TMP})
    string(SUBSTRING ${MAYA_API_VERSION} 0 4 MAYA_API_VERSION_SHORT)
endif()

# handle the QUIETLY and REQUIRED arguments and set MAYA_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Maya DEFAULT_MSG
    ${MAYA_LIBRARIES}
    MAYA_INCLUDE_DIRS)
