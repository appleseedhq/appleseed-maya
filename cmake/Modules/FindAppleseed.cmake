# Copyright (c) 2013 Esteban Tovagliari

# Find Appleseed's headers and libraries.
#
#  This module defines
# APPLESEED_INCLUDE_DIRS - where to find APPLESEED includes.
# APPLESEED_LIBRARIES    - List of libraries when using APPLESEED.
# APPLESEED_FOUND        - True if APPLESEED found.

# Look for the includes.
find_path(APPLESEED_INCLUDE_DIR NAMES renderer/api/project.h)

# Look for the library.
find_library(APPLESEED_LIBRARY NAMES appleseed)

# Look for the OSL compiler
find_program(APPLESEED_OSL_COMPILER NAMES oslc)

# handle the QUIETLY and REQUIRED arguments and set APPLESEED_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    APPLESEED DEFAULT_MSG
    APPLESEED_LIBRARY
    APPLESEED_INCLUDE_DIR
    APPLESEED_OSL_COMPILER)

# Copy the results to the output variables.
if(APPLESEED_FOUND)
    set(APPLESEED_LIBRARIES ${APPLESEED_LIBRARY})
    set(APPLESEED_INCLUDE_DIRS ${APPLESEED_INCLUDE_DIR})
else()
    set(APPLESEED_LIBRARIES)
    set(APPLESEED_INCLUDE_DIRS)
endif()

mark_as_advanced(APPLESEED_LIBRARY APPLESEED_INCLUDE_DIR)
