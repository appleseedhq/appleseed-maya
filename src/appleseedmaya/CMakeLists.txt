
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


configure_file (${PROJECT_SOURCE_DIR}/src/appleseedmaya/version.h.in
                ${PROJECT_SOURCE_DIR}/src/appleseedmaya/version.h)


#--------------------------------------------------------------------------------------------------
# Source files.
#--------------------------------------------------------------------------------------------------

set (appleseed_maya_sources
    _beginmayaheaders.h
    _endmayaheaders.h
    alphamapnode.cpp
    alphamapnode.h
    appleseedsession.cpp
    appleseedsession.h
    appleseedtranslator.cpp
    appleseedtranslator.h
    attributeutils.cpp
    attributeutils.h
    config.h
    envlightdraw.cpp
    envlightdraw.h
    exceptions.h
    extensionattributes.cpp
    extensionattributes.h
    hypershaderenderer.cpp
    hypershaderenderer.h
    idlejobqueue.cpp
    idlejobqueue.h
    logger.cpp
    logger.h
    murmurhash.cpp
    murmurhash.h
    physicalskylightnode.h
    physicalskylightnode.cpp
    pluginmain.cpp
    pythonbridge.cpp
    pythonbridge.h
    ramputils.h
    rendercommands.cpp
    rendercommands.h
    renderercontroller.h
    renderglobalsnode.cpp
    renderglobalsnode.h
    renderviewtilecallback.cpp
    renderviewtilecallback.h
    shadingnode.cpp
    shadingnode.h
    shadingnodemetadata.cpp
    shadingnodemetadata.h
    shadingnoderegistry.cpp
    shadingnoderegistry.h
    shadingnodetemplatebuilder.cpp
    shadingnodetemplatebuilder.h
    skydomelightnode.cpp
    skydomelightnode.h
    swatchrenderer.cpp
    swatchrenderer.h
    typeids.h
    utils.cpp
    utils.h
    version.h.in
)
source_group ("" FILES
    ${appleseed_maya_sources}
)

set (appleseed_maya_exporters_sources
    exporters/alphamapexporter.cpp
    exporters/alphamapexporter.h
    exporters/alphamapexporterfwd.h
    exporters/arealightexporter.cpp
    exporters/arealightexporter.h
    exporters/cameraexporter.cpp
    exporters/cameraexporter.h
    exporters/dagnodeexporter.cpp
    exporters/dagnodeexporter.h
    exporters/dagnodeexporterfwd.h
    exporters/envlightexporter.cpp
    exporters/envlightexporter.h
    exporters/exporterfactory.cpp
    exporters/exporterfactory.h
    exporters/fileexporter.cpp
    exporters/fileexporter.h
    exporters/instanceexporter.cpp
    exporters/instanceexporter.h
    exporters/lightexporter.cpp
    exporters/lightexporter.h
    exporters/mandelbrotexporter.cpp
    exporters/mandelbrotexporter.h
    exporters/meshexporter.cpp
    exporters/meshexporter.h
    exporters/place3dtextureexporter.cpp
    exporters/place3dtextureexporter.h
    exporters/rampexporter.cpp
    exporters/rampexporter.h
    exporters/remapcolorexporter.cpp
    exporters/remapcolorexporter.h
    exporters/remaphsvexporter.cpp
    exporters/remaphsvexporter.h
    exporters/remapvalueexporter.cpp
    exporters/remapvalueexporter.h
    exporters/shadingengineexporter.cpp
    exporters/shadingengineexporter.h
    exporters/shadingengineexporterfwd.h
    exporters/shadingnetworkexporter.cpp
    exporters/shadingnetworkexporter.h
    exporters/shadingnetworkexporterfwd.h
    exporters/shadingnodeexporter.cpp
    exporters/shadingnodeexporter.h
    exporters/shadingnodeexporterfwd.h
    exporters/shapeexporter.cpp
    exporters/shapeexporter.h
)
if (WITH_XGEN)
    set (appleseed_maya_exporters_sources
        ${appleseed_maya_exporters_sources}
        exporters/xgenexporter.cpp
        exporters/xgenexporter.h
    )
endif ()
list (APPEND appleseed_maya_sources
    ${appleseed_maya_exporters_sources}
)
source_group ("exporters" FILES
    ${appleseed_maya_exporters_sources}
)


#--------------------------------------------------------------------------------------------------
# Preprocessor definitions.
#--------------------------------------------------------------------------------------------------

add_definitions (-D_BOOL)
add_definitions (-DREQUIRE_IOSTREAM)

if (WITH_XGEN)
    add_definitions (-DAPPLESEED_MAYA_WITH_XGEN)
endif ()

if (MSVC)
    add_definitions (-D_CRT_SECURE_NO_WARNINGS)
    add_definitions (-D_SCL_SECURE_NO_WARNINGS)
    add_definitions (-DWINDOWS_LEAN_AND_MEAN)
    add_definitions (-DNOMINMAX)
    add_definitions (-DNT_PLUGIN)
    add_definitions (-DBits64_)
    add_definitions (-DARCH64)
elseif (APPLE)
    add_definitions (-DOSMac_)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions (-DLINUX)
endif ()


#--------------------------------------------------------------------------------------------------
# Target.
#--------------------------------------------------------------------------------------------------

add_library (appleseedMaya SHARED
    ${appleseed_maya_sources}
)

set_target_properties (appleseedMaya PROPERTIES PREFIX "")

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set_target_properties (appleseedMaya PROPERTIES SUFFIX ".mll")
elseif (APPLE)
    set_target_properties (appleseedMaya PROPERTIES SUFFIX ".bundle")
endif ()


#--------------------------------------------------------------------------------------------------
# Include paths.
#--------------------------------------------------------------------------------------------------

include_directories (
    ${MAYA_INCLUDE_DIRS}
    ${PYTHON_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src
)


#--------------------------------------------------------------------------------------------------
# Libraries.
#--------------------------------------------------------------------------------------------------

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

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Work around a linking issue caused by Autodesk modifying some definitions in ILMBase's half library:
    # https://groups.google.com/d/msg/vfx-platform-discuss/0gjGDNkYdCo/DRb0-yDmBQAJ
    target_link_libraries (appleseedMaya
        debug       ${APPLESEED_DEPS_STAGE_DIR}/ilmbase-debug/lib/Half.lib
        optimized   ${APPLESEED_DEPS_STAGE_DIR}/ilmbase-release/lib/Half.lib
    )
endif ()
