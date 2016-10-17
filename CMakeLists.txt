
#
# This source file is part of appleseed.
# Visit http://appleseedhq.net/ for additional information and resources.
#
# This software is released under the MIT license.
#
# Copyright (c) 2016 Haggi Krey, The appleseedhq Organization
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


#--------------------------------------------------------------------------------------------------
# CMake configuration.
#--------------------------------------------------------------------------------------------------

cmake_minimum_required (VERSION 2.8 FATAL_ERROR)

project (appleseed-maya2)

set (CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${PROJECT_SOURCE_DIR}/cmake/Modules
)


#--------------------------------------------------------------------------------------------------
# Build options.
#--------------------------------------------------------------------------------------------------

option (USE_STATIC_BOOST "Use static Boost libraries" OFF)


#--------------------------------------------------------------------------------------------------
# Boost libraries.
#--------------------------------------------------------------------------------------------------

set (Boost_MULTITHREADED TRUE)

if (USE_STATIC_BOOST)
    set (Boost_USE_STATIC_LIBS TRUE)
endif ()

set (BOOST_NEEDED_LIBS system thread filesystem)

find_package (Boost 1.55 REQUIRED ${BOOST_NEEDED_LIBS})

add_definitions (-DBOOST_FILESYSTEM_VERSION=3 -DBOOST_FILESYSTEM_NO_DEPRECATED)

include_directories (SYSTEM ${Boost_INCLUDE_DIRS})
link_directories (${Boost_LIBRARY_DIRS})


#--------------------------------------------------------------------------------------------------
# Find external packages.
#--------------------------------------------------------------------------------------------------

find_package (Appleseed)
find_package (OpenGL)

find_package (Maya)
message("Maya API version = ${MAYA_API_VERSION}")


#--------------------------------------------------------------------------------------------------
# Include paths.
#--------------------------------------------------------------------------------------------------

include_directories (${APPLESEED_INCLUDE_DIRS})
include_directories (${MAYA_INCLUDE_DIRS})


#--------------------------------------------------------------------------------------------------
# Preprocessor definitions.
#--------------------------------------------------------------------------------------------------

add_definitions (-D_BOOL)
add_definitions (-DREQUIRE_IOSTREAM)
add_definitions (-DAPPLESEED_WITH_OIIO)
add_definitions (-DAPPLESEED_WITH_OSL)

if (MSVC)
    add_definitions (-D_CRT_SECURE_NO_WARNINGS)
    add_definitions (-D_SCL_SECURE_NO_WARNINGS)
    add_definitions (-DWINDOWS_LEAN_AND_MEAN)
    add_definitions (-DNOMINMAX)
    add_definitions (-DNT_PLUGIN)
    add_definitions (-DBits64_)
    add_definitions (-DARCH64)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions (-DLINUX)
endif ()


#--------------------------------------------------------------------------------------------------
# Products.
#--------------------------------------------------------------------------------------------------

add_subdirectory (src)
