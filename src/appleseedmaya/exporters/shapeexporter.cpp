
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
#include "shapeexporter.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/scene.h"

namespace asf = foundation;
namespace asr = renderer;

ShapeExporter::ShapeExporter(
    const MDagPath&                 path,
    asr::Project&                   project,
    AppleseedSession::SessionMode   sessionMode)
  : DagNodeExporter(path, project, sessionMode)
  , m_numInstances(0)
{
}

ShapeExporter::~ShapeExporter()
{
    if (sessionMode() == AppleseedSession::ProgressiveRenderSession)
    {
        assert(m_objectAssembly.get());

        mainAssembly().assemblies().remove(m_objectAssembly.get());
        mainAssembly().assembly_instances().remove(m_objectAssemblyInstance.get());
    }
}

const asr::TransformSequence& ShapeExporter::transformSequence() const
{
    return m_transformSequence;
}

bool ShapeExporter::supportsInstancing() const
{
    return false;
}

MurmurHash ShapeExporter::hash() const
{
    return MurmurHash();
}

void ShapeExporter::instanceCreated() const
{
    m_numInstances++;
}

asf::AABB3d ShapeExporter::boundingBox() const
{
    asf::AABB3d bbox = objectSpaceBoundingBox(dagPath());
    return m_transformSequence.to_parent(bbox);
}

void ShapeExporter::exportTransformMotionStep(float time)
{
    asf::Matrix4d m = convert(dagPath().inclusiveMatrix());
    asf::Matrix4d invM = convert(dagPath().inclusiveMatrixInverse());
    asf::Transformd xform(m, invM);
    m_transformSequence.set_transform(time, xform);
}

void ShapeExporter::flushEntities()
{
    m_transformSequence.optimize();

    // Create an assembly for this object if needed (instanced or xform motion blur).
    const bool needsAssembly = m_numInstances > 0 || m_transformSequence.size() > 1;
    if (sessionMode() == AppleseedSession::ProgressiveRenderSession || needsAssembly)
    {
        const MString assemblyName = appleseedName() + MString("_assembly");
        m_objectAssembly.reset(
            asr::AssemblyFactory().create(assemblyName.asChar(), asr::ParamArray()));

        mainAssembly().assemblies().insert(m_objectAssembly.release());
        const MString assemblyInstanceName = assemblyName + MString("_instance");

        asr::ParamArray params;
        addVisibilityAttributesToParams(params);
        m_objectAssemblyInstance.reset(
            asr::AssemblyInstanceFactory::create(
                assemblyInstanceName.asChar(),
                params,
                assemblyName.asChar()));

        m_objectAssemblyInstance->transform_sequence() = m_transformSequence;
        mainAssembly().assembly_instances().insert(m_objectAssemblyInstance.release());
    }
}

void ShapeExporter::shapeAttributesToParams(renderer::ParamArray& params)
{
}

void ShapeExporter::createObjectInstance(const MString& objectName)
{
    asr::Assembly* objectAssembly = &mainAssembly();
    asf::Transformd objectInstanceTransform;
    asr::ParamArray params;

    if (m_objectAssembly.get())
    {
        objectAssembly = m_objectAssembly.get();
        objectInstanceTransform = asf::Transformd::identity();
    }
    else
    {
        objectInstanceTransform = m_transformSequence.get_earliest_transform();
        addVisibilityAttributesToParams(params);
    }

    const MString objectInstanceName = appleseedName() + MString("_instance");

    // Get object instance params.
    {
        MString sssSet;
        if (AttributeUtils::get(node(), "asSubsurfaceSet", sssSet))
        {
            if (sssSet.length() != 0)
                params.insert_path("sss_set_id", sssSet.asChar());
        }

        int mediumPriority = 0;
        if (AttributeUtils::get(node(), "asMediumPriority", mediumPriority))
            params.insert("medium_priority", mediumPriority);

        bool isPhotonTarget = false;
        if (AttributeUtils::get(node(), "asIsPhotonTarget", isPhotonTarget))
            params.insert("photon_target", isPhotonTarget);

        float shadowTerminatorCorrection = 0.0f;
        if (AttributeUtils::get(node(), "asShadowTerminatorCorrection", shadowTerminatorCorrection))
            params.insert("shadow_terminator_correction", shadowTerminatorCorrection);
    }

    m_objectInstance.reset(
        asr::ObjectInstanceFactory::create(
            objectInstanceName.asChar(),
            params,
            objectName.asChar(),
            objectInstanceTransform,
            m_frontMaterialMappings,
            m_backMaterialMappings));

    objectAssembly->object_instances().insert(m_objectInstance.release());
}
