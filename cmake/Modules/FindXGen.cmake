# Copyright (c) 2016 Esteban Tovagliari

# Find XGen headers and libraries.
#
#  This module defines
# XGEN_INCLUDE_DIRS - where to find XGEN includes.
# XGEN_LIBRARIES    - List of libraries when using XGEN.
# XGEN_FOUND        - True if XGEN found.

# Look for the includes.
find_path(XGEN_INCLUDE_DIR NAMES XGen/XgConfig.h)

# Look for the library.
find_library(XGEN_LIBRARY NAMES AdskXGen)

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
