
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
#include "xgenexporter.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exporters/exporterfactory.h"

// appleseed.renderer headers.
#include "renderer/api/scene.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MFileObject.h>
#include <maya/MFnDagNode.h>
#include <maya/MGlobal.h>
#include "appleseedmaya/_endmayaheaders.h"

namespace asf = foundation;
namespace asr = renderer;

void XGenExporter::registerExporter()
{
    NodeExporterFactory::registerDagNodeExporter("xgmDescription", &XGenExporter::create);
}

DagNodeExporter* XGenExporter::create(
    const MDagPath&                             path,
    asr::Project&                               project,
    AppleseedSession::SessionMode               sessionMode)
{
    return new XGenExporter(path, project, sessionMode);
}

XGenExporter::XGenExporter(
    const MDagPath&                             path,
    asr::Project&                               project,
    AppleseedSession::SessionMode               sessionMode)
  : DagNodeExporter(path, project, sessionMode)
{
}

XGenExporter::~XGenExporter()
{
    if (sessionMode() == AppleseedSession::ProgressiveRenderSession)
    {
        mainAssembly().assemblies().remove(m_assembly.get());
        mainAssembly().assembly_instances().remove(m_assemblyInstance.get());
    }
}

void XGenExporter::createExporters(const AppleseedSession::Services& services)
{
    // todo: implement this.
}

void XGenExporter::createEntities(
    const AppleseedSession::Options&            options,
    const AppleseedSession::MotionBlurTimes&    motionBlurTimes)
{
    const MString assemblyName = appleseedName() + MString("_assembly");
    asr::ParamArray params;

    // Ported from XGen's sample mtoa extension code.
    // todo: cleanup all the string manipulation code.

    // Get strings based on the current scene name.
    MString currentScene;
    MGlobal::executeCommand("file -q -sn", currentScene);

    MFileObject f;
    f.setRawFullName(currentScene);

    // Path to the directory containing the scene.
    const MString scenePath = f.resolvedPath();

    // Filename of the scene with the extension.
    const MString sceneFile = f.resolvedName();

    // Filename of the scene without the extension.
    const MString sceneName(sceneFile.asChar(), sceneFile.length() - 3);

    params.insert("scene_path", scenePath.asChar());
    params.insert("scene_file", sceneFile.asChar());
    params.insert("scene_name", sceneName.asChar());

    // Get Description and Palette from the dag paths.
    // The current dag path points to the desciption.
    // We get the parent to get the palette name.
    MDagPath palettePath = dagPath();
    palettePath.pop();
    palettePath.pop();

    // Collection name minus the '|' char.
    MString paletteName(palettePath.fullPathName().asChar() + 1);

    MDagPath descriptionPath = dagPath();
    descriptionPath.pop();

    // Description name minus collection name minus 2 '|' chars.
    MString descriptionName(
        descriptionPath.fullPathName().asChar() + paletteName.length() + 2);

    asf::Dictionary patches;
    for(unsigned int i = 0, e = descriptionPath.childCount(); i < e; ++i)
    {
        MDagPath childDagPath;
        MDagPath::getAPathTo(descriptionPath.child(i), childDagPath);

        // Ignore the first child. It should be the description shape
        if (i == 0)
            continue;

        MString patchName(
            childDagPath.fullPathName().asChar() + descriptionName.length() + paletteName.length() + 3);

        // Check that the description matches.
        if (asf::ends_with(patchName.asChar(), descriptionName.asChar()))
        {
            patches.insert(
                asf::to_string(i).c_str(),
                patchName.asChar());
        }
    }

    params.insert("patches", patches);
    m_assembly.reset(
        asr::AssemblyFactory().create(assemblyName.asChar(), params));
}

void XGenExporter::exportTransformMotionStep(float time)
{
    asf::Matrix4d m = convert(dagPath().inclusiveMatrix());
    asf::Matrix4d invM = convert(dagPath().inclusiveMatrixInverse());
    asf::Transformd xform(m, invM);
    m_transformSequence.set_transform(time, xform);
}

void XGenExporter::flushEntities()
{
    mainAssembly().assemblies().insert(m_assembly.release());

    const MString assemblyInstanceName = appleseedName() + MString("_assembly_instance");
    asr::ParamArray params;
    addVisibilityAttributesToParams(params);
    m_assemblyInstance.reset(
        asr::AssemblyInstanceFactory::create(
            assemblyInstanceName.asChar(),
            params,
            m_assembly->get_name()));

    m_assemblyInstance->transform_sequence() = m_transformSequence;
    mainAssembly().assembly_instances().insert(m_assemblyInstance.release());
}
