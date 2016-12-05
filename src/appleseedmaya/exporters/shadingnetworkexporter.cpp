
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016 Esteban Tovagliari, The appleseedhq Organization
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
#include "appleseedmaya/exporters/shadingnetworkexporter.h"

// Standard headers.
#include <algorithm>
#include <set>

// Maya headers.
#include <maya/MItDependencyGraph.h>
#include <maya/MFnDependencyNode.h>

// appleseed.renderer headers.
#include "renderer/api/scene.h"

// appleseed.maya headers.
#include "appleseedmaya/logger.h"

namespace asf = foundation;
namespace asr = renderer;

ShadingNetworkExporter::ShadingNetworkExporter(
    const ShadingNetworkContext   context,
    const MObject&                object,
    const MPlug&                  outputPlug,
    renderer::Assembly&           mainAssembly,
    AppleseedSession::SessionMode sessionMode)
  : m_context(context)
  , m_object(object)
  , m_outputPlug(outputPlug)
  , m_mainAssembly(mainAssembly)
  , m_sessionMode(sessionMode)
{
}

MString ShadingNetworkExporter::shaderGroupName() const
{
    return m_shaderGroup->get_name();
}

void ShadingNetworkExporter::createEntity(const AppleseedSession::Options& options)
{
    MStatus status;

    MFnDependencyNode depNodeFn(m_object);
    MString shaderGroupName = depNodeFn.name() + MString("_shader_group");
    m_shaderGroup = asr::ShaderGroupFactory::create(shaderGroupName.asChar());

    // Add any adaptor shader we need, depending on the context.
    switch(m_context)
    {
        case SurfaceNetworkContext:
        {
            MString layerName = depNodeFn.name() + MString("_outputToCi");
            m_shaderGroup->add_shader("surface", "as_maya_assignToCi", layerName.asChar(), asr::ParamArray());
        }
        break;

        default:
            assert(false);
            RENDERER_LOG_ERROR("Unknown shading network context.");
        break;
    }

    MItDependencyGraph it(
        m_object,
        MFn::kInvalid,
        MItDependencyGraph::kUpstream,
        MItDependencyGraph::kDepthFirst,
        MItDependencyGraph::kNodeLevel,
        &status);

    if(status == MS::kFailure)
    {
        RENDERER_LOG_WARNING(
            "No shading nodes connected to shape %s",
            depNodeFn.name().asChar());
        return;
    }

    // iterate through the output connected shading engines
    std::set<MString, MStringCompareLess> nodesVisited;
    for(; it.isDone() != true; it.next())
    {
        MObject shadingNode = it.thisNode();
        depNodeFn.setObject(shadingNode);

        if(nodesVisited.count(depNodeFn.name()) == 0)
        {
            RENDERER_LOG_INFO("visiting shading node %s", depNodeFn.name().asChar());
            nodesVisited.insert(depNodeFn.name());
            // todo: create node exporter here...
        }
    }

    std::reverse(m_nodeExporters.begin(), m_nodeExporters.end());
}

void ShadingNetworkExporter::flushEntity()
{
    insertEntityWithUniqueName(
        m_mainAssembly.shader_groups(),
        m_shaderGroup);
}
