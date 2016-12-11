
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
#include "appleseedmaya/exporters/xgenexporter.h"

// Maya headers.
#include <maya/MFileObject.h>
#include <maya/MFnDagNode.h>

// appleseed.renderer headers.
#include "renderer/api/scene.h"

// appleseed.maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exporters/exporterfactory.h"

namespace asf = foundation;
namespace asr = renderer;

void XGenExporter::registerExporter()
{
    NodeExporterFactory::registerDagNodeExporter("xgmDescription", &XGenExporter::create);
}

DagNodeExporter *XGenExporter::create(
    const MDagPath&                 path,
    asr::Project&                   project,
    AppleseedSession::SessionMode   sessionMode)
{
    return new XGenExporter(path, project, sessionMode);
}

XGenExporter::XGenExporter(
    const MDagPath&                 path,
    asr::Project&                   project,
    AppleseedSession::SessionMode   sessionMode)
  : DagNodeExporter(path, project, sessionMode)
{
}

void XGenExporter::createExporters(const AppleseedSession::Services& services)
{
    // todo: implement this.
}

void XGenExporter::createEntity(const AppleseedSession::Options& options)
{
    const MString assemblyName = appleseedName() + MString("_assembly");
    asr::ParamArray params;
    // todo: collect xgen info here...
    m_assembly.reset(
        asr::AssemblyFactory().create(assemblyName.asChar(), asr::ParamArray()));
}

void XGenExporter::exportTransformMotionStep(float time)
{
    asf::Matrix4d m = convert(dagPath().inclusiveMatrix());
    asf::Matrix4d invM = convert(dagPath().inclusiveMatrixInverse());
    asf::Transformd xform(m, invM);
    m_transformSequence.set_transform(0.0, xform);
}

void XGenExporter::flushEntity()
{
    const MString assemblyInstanceName = appleseedName() + MString("_assembly_instance");
    asr::ParamArray params;
    visibilityAttributesToParams(params);
    m_assemblyInstance.reset(
        asr::AssemblyInstanceFactory::create(
            assemblyInstanceName.asChar(),
            params,
            m_assembly->get_name()));

    m_assemblyInstance->transform_sequence() = m_transformSequence;
    mainAssembly().assembly_instances().insert(m_assemblyInstance.release());
}
