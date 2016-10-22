
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


configure_file (${PROJECT_SOURCE_DIR}/src/appleseedmaya/version.h.in
                ${PROJECT_SOURCE_DIR}/src/appleseedmaya/version.h)

include_directories (${PROJECT_SOURCE_DIR}/src)

set (appleseed_maya_sources
    appleseedmaya/appleseedsession.cpp
    appleseedmaya/appleseedsession.h
    appleseedmaya/config.h
    appleseedmaya/envlightnode.cpp
    appleseedmaya/envlightnode.h
    appleseedmaya/pluginmain.cpp
    appleseedmaya/rendercommands.cpp
    appleseedmaya/rendercommands.h
    appleseedmaya/renderglobalsnode.cpp
    appleseedmaya/renderglobalsnode.h
    appleseedmaya/shadingnoderegistry.cpp
    appleseedmaya/shadingnoderegistry.h
    appleseedmaya/swatchrenderer.cpp
    appleseedmaya/swatchrenderer.h
    appleseedmaya/version.h.in
)

if (${MAYA_API_VERSION} EQUAL 201600 OR ${MAYA_API_VERSION} GREATER 201600)
    set (appleseed_maya_sources
        ${appleseed_maya_sources}
        appleseedmaya/hypershaderenderer.cpp
        appleseedmaya/hypershaderenderer.h
    )
endif()

add_library (appleseedMaya SHARED
    ${appleseed_maya_sources}
)

target_link_libraries (appleseedMaya
    ${MAYA_Foundation_LIBRARY}
    ${MAYA_OpenMaya_LIBRARY}
    ${MAYA_OpenMayaAnim_LIBRARY}
    ${MAYA_OpenMayaFX_LIBRARY}
    ${MAYA_OpenMayaRender_LIBRARY}
    ${MAYA_OpenMayaUI_LIBRARY}
    ${APPLESEED_LIBRARIES}
    ${Boost_LIBRARIES}
    ${OPENGL_gl_LIBRARY}
)

set_target_properties (appleseedMaya PROPERTIES PREFIX "")

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set_target_properties (appleseedMaya PROPERTIES SUFFIX ".mll")
endif ()
