
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016-2018 Esteban Tovagliari, The appleseedhq Organization
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
#include "instanceexporter.h"

// appleseed-maya headers.
#include "appleseedmaya/appleseedsession.h"

namespace asf = foundation;
namespace asr = renderer;

InstanceExporter::InstanceExporter(
    const MDagPath&                 path,
    AppleseedSession::SessionMode   sessionMode,
    const ShapeExporter&            master,
    asr::Project&                   project,
    const asr::TransformSequence&   transformSequence)
  : ShapeExporter(path, project, sessionMode)
  , m_masterShape(master)
{
    m_transformSequence = transformSequence;
    m_masterShape.instanceCreated();
}

void InstanceExporter::createEntities(
    const AppleseedSession::Options&                options,
    const AppleseedSession::MotionBlurSampleTimes&  motionBlurSampleTimes)
{
    // This exporter instances an already created object.
    // Nothing to do here.
}

void InstanceExporter::flushEntities()
{
    const MString assemblyName = m_masterShape.appleseedName() + MString("_assembly");
    const MString assemblyInstanceName = appleseedName() + MString("_instance");

    asr::ParamArray params;
    addVisibilityAttributesToParams(params);
    asf::auto_release_ptr<asr::AssemblyInstance> assemblyInstance(
        asr::AssemblyInstanceFactory::create(
            assemblyInstanceName.asChar(),
            params,
            assemblyName.asChar()));

    RENDERER_LOG_DEBUG(
        "Flushing assembly instance %s",
        assemblyInstanceName.asChar());

    assemblyInstance->transform_sequence() = m_transformSequence;
    mainAssembly().assembly_instances().insert(assemblyInstance);
}

asf::AABB3d InstanceExporter::boundingBox() const
{
    asf::AABB3d bbox = objectSpaceBoundingBox(m_masterShape.dagPath());
    return m_transformSequence.to_parent(bbox);
}
