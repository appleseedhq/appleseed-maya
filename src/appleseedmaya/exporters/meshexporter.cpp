
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
#include <sstream>

// Boost headers.
#include "boost/filesystem/convenience.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

// Maya headers.
#include <maya/MFloatPointArray.h>
#include <maya/MFnMesh.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MPointArray.h>

// appleseed.foundation headers.
#include "foundation/utility/string.h"

// appleseed.maya headers.
#include "appleseedmaya/exporters/exporterfactory.h"
#include "appleseedmaya/logger.h"

namespace bfs = boost::filesystem;
namespace asf = foundation;
namespace asr = renderer;

// Utility functions
namespace
{

void meshObjectTopologyHash(
    const asr::MeshObject&          mesh,
    const asf::StringDictionary&    materialMappings,
    MurmurHash&                     hash)
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

    hash.append(materialMappings.size());
    asf::StringDictionary::const_iterator it(materialMappings.begin());
    asf::StringDictionary::const_iterator e(materialMappings.end());
    for(;it != e; ++it)
        hash.append(it.value());
}

void staticMeshObjectHash(
    const asr::MeshObject&          mesh,
    const asf::StringDictionary&    materialMappings,
    MurmurHash&                     hash)
{
    meshObjectTopologyHash(mesh, materialMappings, hash);

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

void meshObjectHash(
    const asr::MeshObject&          mesh,
    const asf::StringDictionary&    materialMappings,
    MurmurHash&                     hash)
{
    staticMeshObjectHash(mesh, materialMappings, hash);

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

void MeshExporter::createExporters(const AppleseedSession::Services& services)
{
    const int instanceNumber = dagPath().isInstanced() ? dagPath().instanceNumber() : 0;

    MFnDependencyNode depNodeFn(dagPath().node());
    MPlug plug = depNodeFn.findPlug("instObjGroups");
    plug = plug.elementByLogicalIndex(instanceNumber);

    if(plug.isConnected())
    {
        MPlugArray connections;
        plug.connectedTo(connections, false, true);
        MObject shadingEngine = connections[0].node();
        services.createShadingEngineExporter(shadingEngine);
        depNodeFn.setObject(shadingEngine);
        MString materialName = depNodeFn.name() + MString("_material");
        m_materialMappings.insert("default", materialName.asChar());
    }
    else
    {
        MFnMesh fnMesh(dagPath().node());
        MObjectArray shadingEngines;
        fnMesh.getConnectedShaders(instanceNumber, shadingEngines, m_perFaceAssignments);

        for(size_t i = 0, e = shadingEngines.length(); i < e; ++i)
        {
            services.createShadingEngineExporter(shadingEngines[i]);
            depNodeFn.setObject(shadingEngines[i]);
            MString materialName = depNodeFn.name() + MString("_material");

            if(i == 0)
                m_materialMappings.insert("default", materialName.asChar());
            else
            {
                std::string slotName = asf::get_numbered_string("slot#", i);
                m_materialMappings.insert(slotName.c_str(), materialName.asChar());
            }
        }
    }

    if(m_materialMappings.empty())
    {
        RENDERER_LOG_WARNING(
            "Found mesh %s with no materials.",
            appleseedName().asChar());
    }
}

void MeshExporter::createEntity(const AppleseedSession::Options& options)
{
    shapeAttributesToParams(m_meshParams);
    meshAttributesToParams(m_meshParams);

    MFnMesh meshFn(dagPath());
    m_exportUVs = meshFn.numUVs() != 0;
    m_exportNormals = meshFn.numNormals() != 0;
    m_shapeExportStep = 0;

    if(sessionMode() != AppleseedSession::ExportSession)
    {
        MString objectName = appleseedName();
        m_mesh = asr::MeshObjectFactory::create(objectName.asChar(), m_meshParams);
        createMaterialSlots();
        fillTopology();
    }
}

void MeshExporter::exportShapeMotionStep(float time)
{
    if(sessionMode() == AppleseedSession::ExportSession)
    {
        MString objectName = appleseedName();
        m_mesh = asr::MeshObjectFactory::create(objectName.asChar(), m_meshParams);
        createMaterialSlots();
        fillTopology();
        exportGeometry();

        MurmurHash meshHash;
        staticMeshObjectHash(*m_mesh, m_materialMappings, meshHash);
        const std::string fileName = std::string("_geometry/") + meshHash.toString() + ".binarymesh";

        bfs::path projectPath = project().search_paths().get_root_path();
        bfs::path p = projectPath / fileName;

        // Write a geom file for the object if needed.
        if(!bfs::exists(p))
        {
            if(!asr::MeshObjectWriter::write(*m_mesh, "mesh", p.string().c_str()))
            {
                RENDERER_LOG_ERROR(
                    "Couldn't export mesh file for object %s.",
                    m_mesh->get_name());
            }
        }
        else
        {
            RENDERER_LOG_INFO(
                "Mesh file for object %s already exists.",
                m_mesh->get_name());
        }

        m_fileNames.push_back(fileName);
    }
    else
    {
        if(m_shapeExportStep == 0)
            exportGeometry();
        else
            exportMeshKey();
    }

    m_shapeExportStep++;
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

    RENDERER_LOG_DEBUG("Flushing mesh object %s", m_mesh->get_name());
    if(m_objectAssembly.get())
        m_objectAssembly->objects().insert(m_mesh.releaseAs<asr::Object>());
    else
        mainAssembly().objects().insert(m_mesh.releaseAs<asr::Object>());

    RENDERER_LOG_DEBUG("Flushing object instance %s", m_mesh->get_name());
    createObjectInstance(objectName);
}

void MeshExporter::meshAttributesToParams(renderer::ParamArray& params)
{
    // TODO: implement this...
}

void MeshExporter::createMaterialSlots()
{
    // Create material slots.
    if(!m_materialMappings.empty())
    {
        m_mesh->reserve_material_slots(m_materialMappings.size());
        asf::StringDictionary::const_iterator it(m_materialMappings.begin());
        asf::StringDictionary::const_iterator e(m_materialMappings.end());
        for(;it != e; ++it)
            m_mesh->push_material_slot(it.key());
    }
    else
        m_mesh->push_material_slot("default");
}

void MeshExporter::fillTopology()
{
    // Triangle buffer.
    std::vector<asr::Triangle> triangles;

    MIntArray faceVtxIds;
    MIntArray faceUVIndices;
    MIntArray faceNormalIds;
    MIntArray triVtxIds;
    MPointArray triPoints;

    MItMeshPolygon faceIt(dagPath());
    for(; !faceIt.isDone(); faceIt.next())
    {
        int materialIndex = 0;
        if (m_perFaceAssignments.length() != 0)
            materialIndex = m_perFaceAssignments[faceIt.index()];

        faceUVIndices.clear();
        faceNormalIds.clear();
        faceIt.getVertices(faceVtxIds);

        for(size_t vtxId = 0, vtxEd = faceVtxIds.length(); vtxId < vtxEd; ++vtxId)
        {
            if(m_exportUVs)
            {
                int uvIndex;
                faceIt.getUVIndex(vtxId, uvIndex);
                faceUVIndices.append(uvIndex);
            }

            if(m_exportNormals)
                faceNormalIds.append(faceIt.normalIndex(vtxId));
        }

        int numTris;
        faceIt.numTriangles(numTris);

        for(size_t triId = 0; triId < numTris; ++triId)
        {
            triPoints.clear();
            triVtxIds.clear();
            faceIt.getTriangle(triId, triPoints, triVtxIds);

            int faceRelIds[3];
            for(size_t triVtxId = 0; triVtxId < 3; ++triVtxId)
            {
                for(size_t faceVtxId = 0, faceVtxEd = faceVtxIds.length(); faceVtxId < faceVtxEd; ++faceVtxId)
                {
                    if(faceVtxIds[faceVtxId] == triVtxIds[triVtxId])
                        faceRelIds[triVtxId] = faceVtxId;
                }
            }

            asr::Triangle triangle(
                faceVtxIds[faceRelIds[0]],
                faceVtxIds[faceRelIds[1]],
                faceVtxIds[faceRelIds[2]],
                materialIndex);

            if(m_exportUVs)
            {
                triangle.m_a0 = faceUVIndices[faceRelIds[0]];
                triangle.m_a1 = faceUVIndices[faceRelIds[1]];
                triangle.m_a2 = faceUVIndices[faceRelIds[2]];
            }

            if(m_exportNormals)
            {
                triangle.m_n0 = faceNormalIds[faceRelIds[0]];
                triangle.m_n1 = faceNormalIds[faceRelIds[1]];
                triangle.m_n2 = faceNormalIds[faceRelIds[2]];
            }

            triangles.push_back(triangle);
        }
    }

    // Copy triangles to the mesh.
    m_mesh->reserve_triangles(triangles.size());
    for(size_t i = 0, e = triangles.size(); i < e; ++i)
        m_mesh->push_triangle(triangles[i]);
}

void MeshExporter::exportGeometry()
{
    MStatus status;
    MFnMesh meshFn(dagPath());

    // Vertices.
    m_mesh->reserve_vertices(meshFn.numVertices());
    {
        const float *p = meshFn.getRawPoints(&status);
        for(size_t i = 0, e = meshFn.numVertices(); i < e; ++i)
            m_mesh->push_vertex(asr::GVector3(*p++, *p++, *p++));
    }

    if(m_exportUVs)
    {
        m_mesh->reserve_tex_coords(meshFn.numUVs());
        MFloatArray u, v;
        status = meshFn.getUVs(u, v);
        for(size_t i = 0, e = meshFn.numUVs(); i < e; ++i)
            m_mesh->push_tex_coords(asr::GVector2(u[i], v[i]));
    }

    if(m_exportNormals)
    {
        m_mesh->reserve_vertex_normals(meshFn.numNormals());
        MFloatVectorArray normals;
        status = meshFn.getNormals(normals);
        for(size_t i = 0, e = meshFn.numNormals(); i < e; ++i)
        {
            m_mesh->push_vertex_normal(
                asr::GVector3(normals[i].x, normals[i].y, normals[i].z));
        }
    }
}

void MeshExporter::exportMeshKey()
{

    MStatus status;
    MFnMesh meshFn(dagPath());

    if(m_shapeExportStep == 1)
    {
        // todo: reserve motion steps here...
        //m_mesh->set_motion_segment_count(x);
    }

    // Vertices.
    {
        const float *p = meshFn.getRawPoints(&status);
        for(size_t i = 0, e = meshFn.numVertices(); i < e; ++i)
        {
            m_mesh->set_vertex_pose(
                i,
                m_shapeExportStep - 1,
                asr::GVector3(*p++, *p++, *p++));
        }
    }

    if(m_exportNormals)
    {
        m_mesh->reserve_vertex_normals(meshFn.numNormals());
        MFloatVectorArray normals;
        status = meshFn.getNormals(normals);
        for(size_t i = 0, e = meshFn.numNormals(); i < e; ++i)
        {
            m_mesh->set_vertex_normal_pose(
                i,
                m_shapeExportStep - 1,
                asr::GVector3(normals[i].x, normals[i].y, normals[i].z));
        }
    }
}
