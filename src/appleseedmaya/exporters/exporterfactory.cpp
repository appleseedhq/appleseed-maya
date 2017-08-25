
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016-2017 Esteban Tovagliari, The appleseedhq Organization
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

// Interface header.
#include "exporterfactory.h"

// appleseed-maya headers.
#include "appleseedmaya/exceptions.h"
#include "appleseedmaya/exporters/alphamapexporter.h"
#include "appleseedmaya/exporters/arealightexporter.h"
#include "appleseedmaya/exporters/cameraexporter.h"
#include "appleseedmaya/exporters/envlightexporter.h"
#include "appleseedmaya/exporters/fileexporter.h"
#include "appleseedmaya/exporters/lightexporter.h"
#include "appleseedmaya/exporters/mandelbrotexporter.h"
#include "appleseedmaya/exporters/meshexporter.h"
#include "appleseedmaya/exporters/place3dtextureexporter.h"
#include "appleseedmaya/exporters/rampexporter.h"
#include "appleseedmaya/exporters/remapcolorexporter.h"
#include "appleseedmaya/exporters/remaphsvexporter.h"
#include "appleseedmaya/exporters/remapvalueexporter.h"
#include "appleseedmaya/exporters/shadingengineexporter.h"
#include "appleseedmaya/exporters/shadingnetworkexporter.h"
#include "appleseedmaya/exporters/shadingnodeexporter.h"
#ifdef APPLESEED_MAYA_WITH_XGEN
#include "appleseedmaya/exporters/xgenexporter.h"
#endif
#include "appleseedmaya/logger.h"
#include "appleseedmaya/utils.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MFnDagNode.h>
#include <maya/MFnDependencyNode.h>
#include "appleseedmaya/_endmayaheaders.h"

// Standard headers.
#include <cassert>
#include <map>

namespace asf = foundation;
namespace asr = renderer;

namespace
{
    typedef std::map<
        MString,
        NodeExporterFactory::CreateDagNodeExporterFn,
        MStringCompareLess
        > CreateDagExporterMapType;

    CreateDagExporterMapType            gDagNodeExporters;

    typedef std::map<
        MString,
        NodeExporterFactory::CreateShadingNodeExporterFn,
        MStringCompareLess
        > CreateShadingNodeExporterMapType;

    CreateShadingNodeExporterMapType    gShadingNodeExporters;
}

MStatus NodeExporterFactory::initialize(const MString& pluginPath)
{
    AreaLightExporter::registerExporter();
    CameraExporter::registerExporter();
    LightExporter::registerExporter();
    MeshExporter::registerExporter();
    PhysicalSkyLightExporter::registerExporter();
    SkyDomeLightExporter::registerExporter();
#ifdef APPLESEED_MAYA_WITH_XGEN
    XGenExporter::registerExporter();
#endif

    // Register generic shading node exporters.
    ShadingNodeExporter::registerExporters();

    // Register custom shading node exporters.
    FileExporter::registerExporter();
    Place3dTextureExporter::registerExporter();
    RampExporter::registerExporter();
    RemapColorExporter::registerExporter();
    RemapHsvExporter::registerExporter();
    RemapValueExporter::registerExporter();
    MandelbrotExporter::registerExporter();

    return MS::kSuccess;
}

MStatus NodeExporterFactory::uninitialize()
{
    return MS::kSuccess;
}

void NodeExporterFactory::registerDagNodeExporter(
    const MString&                  mayaTypeName,
    CreateDagNodeExporterFn         createFn)
{
    assert(createFn != nullptr);

    gDagNodeExporters[mayaTypeName] = createFn;

    /*
    RENDERER_LOG_DEBUG(
        "NodeExporterFactory: registered dag node exporter for node %s",
        mayaTypeName.asChar());
    */
}

DagNodeExporter* NodeExporterFactory::createDagNodeExporter(
    const MDagPath&                 path,
    asr::Project&                   project,
    AppleseedSession::SessionMode   sessionMode)
{
    MFnDagNode dagNodeFn(path);
    CreateDagExporterMapType::const_iterator it = gDagNodeExporters.find(dagNodeFn.typeName());

    if (it == gDagNodeExporters.end())
        throw NoExporterForNode();

    return it->second(path, project, sessionMode);
}

ShadingEngineExporter* NodeExporterFactory::createShadingEngineExporter(
    const MObject&                  object,
    renderer::Assembly&             mainAssembly,
    AppleseedSession::SessionMode   sessionMode)
{
    return new ShadingEngineExporter(object, mainAssembly, sessionMode);
}

ShadingNetworkExporter* NodeExporterFactory::createShadingNetworkExporter(
    const ShadingNetworkContext     context,
    const MObject&                  object,
    const MPlug&                    outputPlug,
    renderer::Assembly&             mainAssembly,
    AppleseedSession::SessionMode   sessionMode)
{
    return new ShadingNetworkExporter(
        context,
        object,
        outputPlug,
        mainAssembly,
        sessionMode);
}

void NodeExporterFactory::registerShadingNodeExporter(
    const MString&                  mayaTypeName,
    CreateShadingNodeExporterFn     createFn)
{
    assert(createFn != nullptr);

    gShadingNodeExporters[mayaTypeName] = createFn;

    /*
    RENDERER_LOG_DEBUG(
        "NodeExporterFactory: registered shading node exporter for node %s",
        mayaTypeName.asChar());
    */
}

ShadingNodeExporter* NodeExporterFactory::createShadingNodeExporter(
    const MObject&                  object,
    asr::ShaderGroup&               shaderGroup)
{
    MFnDependencyNode depNodeFn(object);
    CreateShadingNodeExporterMapType::const_iterator it = gShadingNodeExporters.find(depNodeFn.typeName());

    if (it == gShadingNodeExporters.end())
        throw NoExporterForNode();

    return it->second(object, shaderGroup);
}

AlphaMapExporter*NodeExporterFactory::createAlphaMapExporter(
    const MObject&                  object,
    asr::Project&                   project,
    AppleseedSession::SessionMode   sessionMode)
{
    return AlphaMapExporter::create(object, project, sessionMode);
}
