
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
#include "xgenexporter.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exporters/exporterfactory.h"

// appleseed.renderer headers.
#include "renderer/api/scene.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MAnimControl.h>
#include <maya/MFileObject.h>
#include <maya/MFnDagNode.h>
#include <maya/MGlobal.h>
#include <maya/MTime.h>
#include "appleseedmaya/_endmayaheaders.h"

// Standard headers.
#include <map>
#include <string>

namespace asf = foundation;
namespace asr = renderer;

void XGenExporter::registerExporter()
{
    NodeExporterFactory::registerDagNodeExporter("xgmDescription", &XGenExporter::create);
}

DagNodeExporter* XGenExporter::create(
    const MDagPath&                                 path,
    asr::Project&                                   project,
    AppleseedSession::SessionMode                   sessionMode)
{
    return new XGenExporter(path, project, sessionMode);
}

XGenExporter::XGenExporter(
    const MDagPath&                                 path,
    asr::Project&                                   project,
    AppleseedSession::SessionMode                   sessionMode)
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

void XGenExporter::createExporters(const AppleseedSession::IExporterFactory& exporter_factory)
{
    // todo: implement this.
}

namespace
{

float getUnitConversionFactor()
{
    static std::map<std::string, float> unit_conversions;
    if (unit_conversions.empty())
    {
        unit_conversions["in"] = 2.54f;
        unit_conversions["ft"] = 30.48f;
        unit_conversions["yd"] = 91.44f;
        unit_conversions["mi"] = 160934.4f;
        unit_conversions["mm"] = 0.1f;
        unit_conversions["km"] = 100000.0f;
        unit_conversions["m"] =  100.0f;
        unit_conversions["dm"] = 10.0;
    }

    MString currentMayaUnit;
    MGlobal::executeCommand("currentUnit -q -linear", currentMayaUnit);

    auto it = unit_conversions.find(currentMayaUnit.asChar());
    if(it != unit_conversions.end())
        return it->second;

    return 1.0f;
}

float getFps()
{
    float fps = 24.0f;
    const MTime::Unit unit = MTime::uiUnit();

    if (unit != MTime::kInvalid)
    {
        const MTime time(1.0, MTime::kSeconds);
        fps = static_cast<float>(time.as(unit));
    }

    if (fps <= 0.f )
    {
        fps = 24.0f;
    }

    return fps;
}

}

void XGenExporter::createEntities(
    const AppleseedSession::Options&                options,
    const AppleseedSession::MotionBlurSampleTimes&  motionBlurSampleTimes)
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

    // Create an assembly.
    m_assembly.reset(asr::AssemblyFactory().create(assemblyName.asChar(), params));

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

    // Build the XGen arguments string.
    std::string xgen_args;
    xgen_args  = "-debug 1 -warning 1 -stats 1 ";
    xgen_args += asf::format(" -frame {0}", MAnimControl::currentTime().value());
    xgen_args += asf::format(" -fps {0}", getFps());
    xgen_args += asf::format(" -file {0}{1}__{2}.xgen", scenePath.asChar(), sceneName.asChar(), paletteName.asChar());
    xgen_args += asf::format(" -palette {0}", paletteName.asChar());
    xgen_args += asf::format(" -geom {0}{1}__{2}.abc", scenePath.asChar(), sceneName.asChar(), paletteName.asChar());
    xgen_args += " -patch {0}";
    xgen_args += asf::format(" -description {0}", descriptionName.asChar());
    xgen_args += asf::format(" -world {0};0;0;0;0;{0};0;0;0;0;{0};0;0;0;0;1", getUnitConversionFactor());

    for (unsigned int i = 0, e = descriptionPath.childCount(); i < e; ++i)
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
            // Create an assembly for the patch.
            params.clear();
            params.insert("plugin_name", "xgenseed");

            // Remove the description name from the patch name.
            patchName = patchName.substring(0, patchName.length() - descriptionName.length() - 2);

            params.insert_path(
                "parameters.xgen_args", asf::format(xgen_args, patchName.asChar()).c_str());

            const asr::AssemblyFactoryRegistrar& assemblyFactories =
                project().get_factory_registrar<asr::Assembly>();

            const auto factory = assemblyFactories.lookup("xgen_patch_assembly");
            assert(factory);

            AppleseedEntityPtr<renderer::Assembly> patchAssembly(
                factory->create(patchName.asChar(), params));

            // Create an assembly instance for the patch.
            const MString patchInstanceName = patchName + "_instance";
            AppleseedEntityPtr<renderer::AssemblyInstance> patchAssemblyInstance(
                asr::AssemblyInstanceFactory().create(
                    patchInstanceName.asChar(),
                    asr::ParamArray(),
                    patchName.asChar()));

            m_assembly->assemblies().insert(patchAssembly.release());
            m_assembly->assembly_instances().insert(patchAssemblyInstance.release());
        }
    }
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
