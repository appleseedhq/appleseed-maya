
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
    appleseedmaya/appleseedtranslator.cpp
    appleseedmaya/appleseedtranslator.h
    appleseedmaya/attributeutils.cpp
    appleseedmaya/attributeutils.h
    appleseedmaya/config.h
    appleseedmaya/envlightnode.cpp
    appleseedmaya/envlightnode.h
    appleseedmaya/exporters/cameraexporter.cpp
    appleseedmaya/exporters/cameraexporter.h
    appleseedmaya/exporters/envlightexporter.cpp
    appleseedmaya/exporters/envlightexporter.h
    appleseedmaya/exporters/dagnodeexporter.cpp
    appleseedmaya/exporters/dagnodeexporter.h
    appleseedmaya/exporters/exporterfactory.cpp
    appleseedmaya/exporters/exporterfactory.h
    appleseedmaya/exporters/instanceexporter.cpp
    appleseedmaya/exporters/instanceexporter.h
    appleseedmaya/exporters/lightexporter.cpp
    appleseedmaya/exporters/lightexporter.h
    appleseedmaya/exporters/meshexporter.cpp
    appleseedmaya/exporters/meshexporter.h
    appleseedmaya/exporters/mpxnodeexporter.cpp
    appleseedmaya/exporters/mpxnodeexporter.h
    appleseedmaya/exporters/shadingnetworkexporter.cpp
    appleseedmaya/exporters/shadingnetworkexporter.h
    appleseedmaya/exporters/shapeexporter.cpp
    appleseedmaya/exporters/shapeexporter.h
    appleseedmaya/murmurhash.cpp
    appleseedmaya/murmurhash.h
    appleseedmaya/pluginmain.cpp
    appleseedmaya/rendercommands.cpp
    appleseedmaya/rendercommands.h
    appleseedmaya/renderercontroller.h
    appleseedmaya/renderglobalsnode.cpp
    appleseedmaya/renderglobalsnode.h
    appleseedmaya/shadingnode.cpp
    appleseedmaya/shadingnode.h
    appleseedmaya/shadingnodemetadata.cpp
    appleseedmaya/shadingnodemetadata.h
    appleseedmaya/shadingnoderegistry.cpp
    appleseedmaya/shadingnoderegistry.h
    appleseedmaya/shadingnodetemplatebuilder.cpp
    appleseedmaya/shadingnodetemplatebuilder.h
    appleseedmaya/swatchrenderer.cpp
    appleseedmaya/swatchrenderer.h
    appleseedmaya/utils.cpp
    appleseedmaya/utils.h
    appleseedmaya/version.h.in
)

if (WITH_PYTHON_BRIDGE)
    set (appleseed_maya_sources
        ${appleseed_maya_sources}
        appleseedmaya/python.cpp
        appleseedmaya/python.h
    )
endif ()

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
    ${PYTHON_LIBRARIES}
)

set_target_properties (appleseedMaya PROPERTIES PREFIX "")

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set_target_properties (appleseedMaya PROPERTIES SUFFIX ".mll")
endif ()
