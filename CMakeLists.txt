
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


#--------------------------------------------------------------------------------------------------
# CMake configuration.
#--------------------------------------------------------------------------------------------------

cmake_minimum_required (VERSION 2.8 FATAL_ERROR)

project (appleseed-maya)

set (CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${PROJECT_SOURCE_DIR}/cmake/Modules
)


#--------------------------------------------------------------------------------------------------
# Create the build configurations.
#--------------------------------------------------------------------------------------------------

set (CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Prevent in-source builds.
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message (FATAL_ERROR "In-source builds are not permitted; run CMake inside an empty build directory.")
endif ()

# Select a build configuration if none is selected yet.
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Debug
         CACHE STRING "Choose the type of build, options are: ${CMAKE_CONFIGURATION_TYPES}" FORCE)
    set_property (CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})
endif ()


#--------------------------------------------------------------------------------------------------
# Version.
#--------------------------------------------------------------------------------------------------

set (appleseed_maya_version_major    0)
set (appleseed_maya_version_minor    1)
set (appleseed_maya_version_patch    0)


#--------------------------------------------------------------------------------------------------
# Build options.
#--------------------------------------------------------------------------------------------------

option (USE_STATIC_BOOST    "Use static Boost libraries" ON)
option (WITH_XGEN           "Build XGen support"         OFF)
option (WITH_PYTHON_BRIDGE  "Build Python bridge"        OFF)


#--------------------------------------------------------------------------------------------------
# Compile flags.
#--------------------------------------------------------------------------------------------------

if (UNIX)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif ()


#--------------------------------------------------------------------------------------------------
# Boost libraries.
#--------------------------------------------------------------------------------------------------

set (Boost_MULTITHREADED TRUE)

if (USE_STATIC_BOOST)
    set (Boost_USE_STATIC_LIBS TRUE)
endif ()

set (BOOST_NEEDED_LIBS filesystem system)

if (WITH_PYTHON_BRIDGE)
    set (BOOST_NEEDED_LIBS ${BOOST_NEEDED_LIBS} python)
endif ()

find_package (Boost 1.55 REQUIRED ${BOOST_NEEDED_LIBS})

add_definitions (-DBOOST_FILESYSTEM_VERSION=3 -DBOOST_FILESYSTEM_NO_DEPRECATED)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Link statically against Boost.Python on Windows.
    add_definitions (-DBOOST_PYTHON_STATIC_LIB)
endif ()

if (NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Workaround for undefined reference to boost::filesystem::detail::copy_file link error
    # on Linux and macOS if Boost is built in C++03 mode.
    add_definitions (-DBOOST_NO_CXX11_SCOPED_ENUMS)
endif ()

include_directories (SYSTEM ${Boost_INCLUDE_DIRS})
link_directories (${Boost_LIBRARY_DIRS})


#--------------------------------------------------------------------------------------------------
# Find external packages.
#--------------------------------------------------------------------------------------------------

find_package (Appleseed REQUIRED)
find_package (OpenGL REQUIRED)
find_package (Imath REQUIRED)
find_package (OpenImageIO REQUIRED)
find_package (OSL REQUIRED)

find_package (Maya REQUIRED)
message ("Maya API version = ${MAYA_API_VERSION}")

if (WITH_XGEN)
    find_package (XGen REQUIRED)
endif ()

if (WITH_PYTHON_BRIDGE)
    find_package (PythonLibs REQUIRED)
endif ()


#--------------------------------------------------------------------------------------------------
# Common include paths.
#--------------------------------------------------------------------------------------------------

include_directories (
    ${APPLESEED_INCLUDE_DIRS}
)


#--------------------------------------------------------------------------------------------------
# Products.
#--------------------------------------------------------------------------------------------------

add_subdirectory (src/appleseedmaya)

if (WITH_XGEN)
    add_subdirectory (src/xgenseed)
endif ()
