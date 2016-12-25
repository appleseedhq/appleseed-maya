
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
#include "appleseedmaya/exporters/shapeexporter.h"

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
        // todo: cleanup here.
    }
}

const asr::TransformSequence& ShapeExporter::transformSequence() const
{
    return m_transformSequence;
}

void ShapeExporter::instanceCreated() const
{
    m_numInstances++;
}

void ShapeExporter::exportTransformMotionStep(float time)
{
    asf::Matrix4d m = convert(dagPath().inclusiveMatrix());
    asf::Matrix4d invM = convert(dagPath().inclusiveMatrixInverse());
    asf::Transformd xform(m, invM);
    m_transformSequence.set_transform(0.0, xform);
}

void ShapeExporter::flushEntity()
{
    m_transformSequence.optimize();

    // Check if we need to create an assembly for this object.
    if (sessionMode() == AppleseedSession::ProgressiveRenderSession ||
       m_numInstances > 0 || m_transformSequence.size() > 1)
    {
        const MString assemblyName = appleseedName() + MString("_assembly");
        m_objectAssembly.reset(
            asr::AssemblyFactory().create(assemblyName.asChar(), asr::ParamArray()));

        mainAssembly().assemblies().insert(m_objectAssembly.release());

        const MString assemblyInstanceName = assemblyName + MString("_instance");

        asr::ParamArray params;
        visibilityAttributesToParams(params);
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
    //m_objectInstance->get_parameters().insert( "medium_priority", appleseedAttributes->m_mediumPriority );

    // TODO: implement this...
}

void ShapeExporter::createObjectInstance(const MString& objectName)
{
    asr::Assembly *objectAssembly = &mainAssembly();
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
        visibilityAttributesToParams(params);
    }

    const MString objectInstanceName = appleseedName() + MString("_instance");
    m_objectInstance.reset(
        asr::ObjectInstanceFactory::create(
            objectInstanceName.asChar(),
            params,
            objectName.asChar(),
            objectInstanceTransform,
            m_materialMappings,
            m_materialMappings));

    objectAssembly->object_instances().insert(m_objectInstance.release());
}
