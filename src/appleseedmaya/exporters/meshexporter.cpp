
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
#include "appleseedmaya/exporters/meshexporter.h"

// Standard headers.
#include <iostream>

// Boost headers.
#include "boost/filesystem/path.hpp"

// Maya headers.
#include <maya/MFnMesh.h>
#include <maya/MIntArray.h>

// appleseed.maya headers.
#include "appleseedmaya/appleseedsession.h"
#include "appleseedmaya/exporters/exporterfactory.h"

namespace bfs = boost::filesystem;
namespace asf = foundation;
namespace asr = renderer;

void MeshExporter::registerExporter()
{
    NodeExporterFactory::registerDagNodeExporter("mesh", &MeshExporter::create);
}

DagNodeExporter *MeshExporter::create(const MDagPath& path, asr::Project& project)
{
    return new MeshExporter(path, project);
}

MeshExporter::MeshExporter(const MDagPath& path, asr::Project& project)
  : ShapeExporter(path, project)
{
}

void MeshExporter::collectDependencyNodesToExport(MObjectArray& nodes)
{
    //MFnMesh meshFn(dagPath());

    // while testing...
    //m_materialMappings.insert("default", "lambert1");

    /*
    MObjectArray shaders;
    MIntArray indices;
    meshFn.getConnectedShaders(dagPath().instanceNumber(), shaders, indices);

    // No materials assigned.
    if(shaders.length() == 0)
        return;

    // For now we only save the first material.
    MFnDependencyNode nodeFn(shaders[0]);
    m_materialMappings.insert("default", nodeFn.name().asChar());
    collectMaterial(shaders[0], nodes);
    */
}

void MeshExporter::createEntity()
{
    MString objectName = appleseedName();

    m_mesh = asr::MeshObjectFactory::create(objectName.asChar(), asr::ParamArray());

    // Todo: create topology here...
    /*
    MFnMesh meshFn(dagPath());
    m_mesh->reserve_vertices(meshFn.numVertices());
    m_mesh->reserve_vertex_normals(meshFn.numNormals());
    m_mesh->reserve_tex_coords(meshFn.numUVs());

    MIntArray triangleCounts;
    MIntArray triangleVertices;
    meshFn.getTriangles(triangleCounts, triangleVertices);
    m_mesh->reserve_triangles(triangleVertices.length() / 3);
    */

    /*
    asr::ParamArray surfaceShaderParams;
    m_surfaceShader = asr::PhysicalSurfaceShaderFactory().create(
        name.asChar(),
        surfaceShaderParams);

    asr::ParamArray materialParams;
    materialParams.insert("surface_shader", name.asChar());
    m_material = asr::OSLMaterialFactory().create(name.asChar(), materialParams);
    */
}

void MeshExporter::exportShapeMotionStep(float time)
{
    // todo: fill geom info here...

    if(AppleseedSession::mode() == AppleseedSession::ExportSession)
    {
        // export mesh here...
        bfs::path projPath = project().search_paths().get_root_path();
        bfs::path geomPath = projPath / "_geometry";

        MurmurHash meshHash;
        // todo: compute mesh hash here.

        /*
        if(mesh file does not exist)
        {
            save mesh file here...
        }
        */

        // update the params dict.
    }
}

void MeshExporter::flushEntity()
{
    ShapeExporter::flushEntity();

    //mainAssembly().surface_shaders().insert(m_surfaceShader.release());
    //mainAssembly().materials().insert(m_material.release());

    const MString objectInstanceName = appleseedName() + MString("_instance");

    asf::StringDictionary materials;
    //materials.insert("default", g_defaultMaterialName);

    AppleseedEntityPtr<asr::ObjectInstance> meshInstance;
    meshInstance = asr::ObjectInstanceFactory::create(
        objectInstanceName.asChar(),
        asr::ParamArray(),
        m_mesh->get_name(),
        m_transformSequence.get_earliest_transform(),
        materials,
        materials);

    std::cout << "Flushing mesh: " << m_mesh->get_name() << std::endl;
    mainAssembly().objects().insert(m_mesh.releaseAs<asr::Object>());

    std::cout << "Flushing instance: " << objectInstanceName << std::endl;
    mainAssembly().object_instances().insert(meshInstance.release());
}
