
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016-2019 Esteban Tovagliari, The appleseedhq Organization
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
#include "shadingengineexporter.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exporters/shadingnetworkexporter.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/scene.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MFnDependencyNode.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include "appleseedmaya/_endmayaheaders.h"

namespace asf = foundation;
namespace asr = renderer;

ShadingEngineExporter::ShadingEngineExporter(
    const MObject&                  object,
    asr::Assembly&                  mainAssembly,
    AppleseedSession::SessionMode   sessionMode)
  : m_sessionMode(sessionMode)
  , m_object(object)
  , m_mainAssembly(mainAssembly)
{
}

ShadingEngineExporter::~ShadingEngineExporter()
{
    if (m_sessionMode == AppleseedSession::ProgressiveRenderSession)
    {
        m_mainAssembly.materials().remove(m_material.get());

        if (m_surfaceShader.get())
            m_mainAssembly.surface_shaders().remove(m_surfaceShader.get());
    }
}

void ShadingEngineExporter::createExporters(const AppleseedSession::IExporterFactory& exporter_factory)
{
    MFnDependencyNode depNodeFn(m_object);

    MStatus status;
    const MPlug plug = depNodeFn.findPlug("surfaceShader", /*wantNetworkedPlug=*/ false, &status);
    if (plug.isConnected())
    {
        MPlugArray otherPlugs;
        plug.connectedTo(otherPlugs, true, false, &status);
        if (otherPlugs.length() == 1)
        {
            MObject otherNode = otherPlugs[0].node();
            depNodeFn.setObject(otherNode);
            m_surfaceNetworkExporter = exporter_factory.createShadingNetworkExporter(
                SurfaceNetworkContext,
                otherNode,
                otherPlugs[0]);
        }
    }
    else if (plug.numConnectedChildren() != 0)
    {
        RENDERER_LOG_WARNING("Unsupported component connection to shading engine.");
    }
}

void ShadingEngineExporter::createEntities(const AppleseedSession::Options& options)
{
    MFnDependencyNode depNodeFn(m_object);
    const MString appleseedName = depNodeFn.name();

    // Create a surface shader if needed.
    int numSamples = 1;
    AttributeUtils::get(depNodeFn, "asShadingSamples", numSamples);

    if (numSamples > 1)
    {
        MString surfaceShaderName = appleseedName + MString("_surface_shader");
        m_surfaceShader.reset(
            asr::PhysicalSurfaceShaderFactory().create(
                surfaceShaderName.asChar(),
                asr::ParamArray().insert("lighting_samples", numSamples)));
    }

    // Create the material.
    MString materialName = appleseedName + MString("_material");
    m_material.reset(asr::OSLMaterialFactory().create(
        materialName.asChar(), asr::ParamArray()));

    // Set the surface shader in the material.
    if (m_surfaceShader.get())
        m_material->get_parameters().insert("surface_shader", m_surfaceShader->get_name());
}

void ShadingEngineExporter::flushEntities()
{
    if (m_surfaceShader.get())
        m_mainAssembly.surface_shaders().insert(m_surfaceShader.release());

    if (m_surfaceNetworkExporter)
    {
        m_material->get_parameters().insert(
            "osl_surface",
            m_surfaceNetworkExporter->shaderGroupName().asChar());
    }

    m_mainAssembly.materials().insert(m_material.release());
}
