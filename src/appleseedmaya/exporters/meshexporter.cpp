
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
#include <sstream>

// Boost headers.
#include "boost/filesystem/convenience.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

// Maya headers.
#include <maya/MFloatPointArray.h>
#include <maya/MFnMesh.h>
#include <maya/MIntArray.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MItMeshPolygon.h>

// appleseed.maya headers.
#include "appleseedmaya/exporters/exporterfactory.h"


namespace bfs = boost::filesystem;
namespace asf = foundation;
namespace asr = renderer;

// Utility functions
namespace
{

void meshObjectTopologyHash(const asr::MeshObject& mesh, MurmurHash& hash)
{
    hash.append(mesh.get_tex_coords_count());
    for(int i = 0, e = mesh.get_tex_coords_count(); i < e; ++i)
        hash.append(mesh.get_tex_coords(i));

    hash.append(mesh.get_triangle_count());
    for(int i = 0, e = mesh.get_triangle_count(); i < e; ++i)
        hash.append(mesh.get_triangle(i));

    hash.append(mesh.get_material_slot_count());
    for(int i = 0, e = mesh.get_material_slot_count(); i < e; ++i)
        hash.append(mesh.get_material_slot(i));
}

void staticMeshObjectHash(const asr::MeshObject& mesh, MurmurHash& hash)
{
    meshObjectTopologyHash(mesh, hash);

    hash.append(mesh.get_vertex_count());
    for(int i = 0, e = mesh.get_vertex_count(); i < e; ++i)
        hash.append(mesh.get_vertex(i));

    hash.append(mesh.get_vertex_normal_count());
    for(int i = 0, e = mesh.get_vertex_normal_count(); i < e; ++i)
        hash.append(mesh.get_vertex_normal(i));

    hash.append(mesh.get_vertex_tangent_count());
    for(int i = 0, e = mesh.get_vertex_tangent_count(); i < e; ++i)
        hash.append(mesh.get_vertex_tangent(i));
}

void meshObjectHash(const asr::MeshObject& mesh, MurmurHash& hash)
{
    staticMeshObjectHash(mesh, hash);

    hash.append(mesh.get_motion_segment_count());
    for(int j = 0, je = mesh.get_motion_segment_count(); j < je; ++j)
    {
        for(int i = 0, e = mesh.get_vertex_count(); i < e; ++i)
            mesh.get_vertex_pose(i, j);

        for(int i = 0, e = mesh.get_vertex_normal_count(); i < e; ++i)
            mesh.get_vertex_normal_pose(i, j);

        for(int i = 0, e = mesh.get_vertex_tangent_count(); i < e; ++i)
            mesh.get_vertex_tangent_pose(i, j);
    }
}

} // unnamed.

void MeshExporter::registerExporter()
{
    NodeExporterFactory::registerDagNodeExporter("mesh", &MeshExporter::create);
}

DagNodeExporter *MeshExporter::create(
    const MDagPath&                 path,
    asr::Project&                   project,
    AppleseedSession::SessionMode   sessionMode)
{
    return new MeshExporter(path, project, sessionMode);
}

MeshExporter::MeshExporter(
    const MDagPath&                 path,
    asr::Project&                   project,
    AppleseedSession::SessionMode   sessionMode)
  : ShapeExporter(path, project, sessionMode)
{
}

void MeshExporter::collectDependencyNodesToExport(MObjectArray& nodes)
{
    MStatus status;

    MObject obj = dagPath().node();
    MItDependencyGraph it(
        obj,
        MFn::kShadingEngine,
        MItDependencyGraph::kDownstream,
        MItDependencyGraph::kBreadthFirst,
        MItDependencyGraph::kNodeLevel,
        &status);

    if(status == MS::kFailure)
    {
        std::cout << "No shading engines connected to shape " << appleseedName() << std::endl;
        return;
    }

    // we want to prune the iteration if the node is not a shading engine
    it.enablePruningOnFilter();

    // iterate through the output connected shading engines
    for(; it.isDone() != true; it.next())
    {
        MObject shadingEngine = it.thisNode();
        nodes.append(shadingEngine);
    }

    // TODO: create material dict here...
}

void MeshExporter::createEntity(const AppleseedSession::Options& options)
{
    asr::ParamArray params;
    shapeAttributesToParams(params);
    meshAttributesToParams(params);

    MString objectName = appleseedName();
    m_mesh = asr::MeshObjectFactory::create(objectName.asChar(), params);

    /*
    // Create material slots.
    {
        m_mesh->reserve_material_slots(m_materialMappings.size());

        asf::StringDictionary::const_iterator it(m_materialMappings.begin());
        asf::StringDictionary::const_iterator e(m_materialMappings.end());
        for(;it != e; ++it)
        {
        }
    }
    */
    m_mesh->push_material_slot("default");

    // For now, set to false...
    m_exportUVs = false;
    m_exportNormals = false;

    fillTopology();
}

void MeshExporter::exportShapeMotionStep(float time)
{
    exportGeometry();

    if(sessionMode() == AppleseedSession::ExportSession)
    {
        MurmurHash meshHash;
        staticMeshObjectHash(*m_mesh, meshHash);
        const std::string fileName = std::string("_geometry/") + meshHash.toString() + ".binarymesh";

        bfs::path projectPath = project().search_paths().get_root_path();
        bfs::path p = projectPath / fileName;

        // Write a geom file for the object if needed.
        if(!bfs::exists(p))
        {
            if(!asr::MeshObjectWriter::write(*m_mesh, "mesh", p.string().c_str()))
            {
                std::cout << "Couldn't export mesh file for object " << m_mesh->get_name() << std::endl;
            }
        }

        m_fileNames.push_back(fileName);
    }
    else
    {
        // Update motion step here...
    }
}

void MeshExporter::flushEntity()
{
    ShapeExporter::flushEntity();

    MString objectName = appleseedName();

    if(sessionMode() == AppleseedSession::ExportSession)
    {
        assert(!m_fileNames.empty());

        // Replace our MeshObject by one referencing the exported meshes.
        asr::ParamArray params = m_mesh->get_parameters();

        if(m_fileNames.size() == 1)
            params.insert("filename", m_fileNames[0].c_str());
        else
        {
            asf::Dictionary fileNames;
            MString key;

            for(int i = 0, e = m_fileNames.size(); i < e; ++i)
            {
                key.set(static_cast<double>(i));
                fileNames.insert(key.asChar(), m_fileNames[i].c_str());
            }

            params.insert("filenames", fileNames);
        }

        m_mesh.reset(asr::MeshObjectFactory().create(m_mesh->get_name(), params));
        objectName += ".mesh";
    }

    std::cout << "Flushing mesh: " << m_mesh->get_name() << std::endl;
    if(m_objectAssembly.get())
        m_objectAssembly->objects().insert(m_mesh.releaseAs<asr::Object>());
    else
        mainAssembly().objects().insert(m_mesh.releaseAs<asr::Object>());

    std::cout << "Flushing object instance: " << m_mesh->get_name() << std::endl;
    createObjectInstance(objectName);
}

void MeshExporter::meshAttributesToParams(renderer::ParamArray& params)
{
    // TODO: implement this...
}

void MeshExporter::fillTopology()
{
    MFnMesh meshFn(dagPath());

    bool exportUVs = false;
    bool exportNormals = false;

    MIntArray triangleCounts;
    MIntArray triangleVertices;
    meshFn.getTriangles(triangleCounts, triangleVertices);

    const size_t numFaces = triangleCounts.length();
    const size_t numTriangles = triangleVertices.length() / 3;
    m_mesh->reserve_triangles(numTriangles);

    size_t vertexIndex = 0;
    for(size_t faceIndex = 0; faceIndex < numFaces; ++faceIndex)
    {
        const size_t numTrianglesPerFace = triangleCounts[faceIndex];
        for(int triangleIndex = 0; triangleIndex < numTrianglesPerFace; ++triangleIndex)
        {
            asr::Triangle triangle(
                triangleVertices[vertexIndex],
                triangleVertices[vertexIndex + 1],
                triangleVertices[vertexIndex + 2]);

            if(m_exportUVs)
            {
                // TODO: set UVs indices here...
            }

            if(m_exportNormals)
            {
                // TODO: set normal indices here...
            }

            // TODO: get face material index here...

            m_mesh->push_triangle(triangle);
            vertexIndex += 3;
        }
    }
}

void MeshExporter::exportGeometry()
{
    MStatus status;
    MFnMesh meshFn(dagPath());

    // Vertices.
    m_mesh->reserve_vertices(meshFn.numVertices());
    {
        MFloatPointArray points;
        status = meshFn.getPoints(points);

        for(size_t i = 0, e = meshFn.numVertices(); i < e; ++i)
            m_mesh->push_vertex(asr::GVector3(points[i].x, points[i].y, points[i].z));
    }

    if(m_exportUVs)
    {
        // m_mesh->reserve_tex_coords(meshFn.numUVs());
        // ...
    }

    if(m_exportNormals)
    {
        // m_mesh->reserve_vertex_normals(meshFn.numNormals());
        // ...
    }
}
