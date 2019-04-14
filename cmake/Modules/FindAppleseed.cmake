#
# This source file is part of appleseed.
# Visit https://appleseedhq.net/ for additional information and resources.
#
# This software is released under the MIT license.
#
# Copyright (c) 2013-2019 Esteban Tovagliari, The appleseedhq Organization
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

# Find appleseed's headers and libraries.
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
