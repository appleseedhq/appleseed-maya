
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
    if(areObjectAndParentsRenderable(path) == false)
        return 0;

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

//#define APPLESEED_MAYA_OBJ_MESH_EXPORT
#ifdef APPLESEED_MAYA_OBJ_MESH_EXPORT
        const char *extension = ".obj";
#else
        const char *extension = ".binarymesh";
#endif
        const std::string fileName = std::string("_geometry/") + meshHash.toString() + extension;

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
    MStatus status;

    // Triangle buffer.
    std::vector<asr::Triangle> triangles;

    MIntArray faceVertexIndices;
    MIntArray faceUVIndices;
    MIntArray faceNormalIndices;
    MIntArray triangleVertexIndices;
    MPointArray trianglePoints;

    MItMeshPolygon faceIt(dagPath());
    for(; !faceIt.isDone(); faceIt.next())
    {
        // Get the material index for this face.
        int materialIndex = 0;
        if (m_perFaceAssignments.length() != 0)
            materialIndex = m_perFaceAssignments[faceIt.index()];

        // Collect normal and uv indices for this face.
        faceUVIndices.clear();
        faceNormalIndices.clear();

        faceIt.getVertices(faceVertexIndices);
        for(size_t i = 0, e = faceVertexIndices.length(); i < e; ++i)
        {
            if(m_exportUVs)
            {
                int uvIndex;
                status = faceIt.getUVIndex(i, uvIndex);
                faceUVIndices.append(uvIndex);
            }

            if(m_exportNormals)
            {
                unsigned int normalIndex = faceIt.normalIndex(i, &status);
                faceNormalIndices.append(normalIndex);
            }
        }

        // Match the triangle indices to the face indices.
        int numTris;
        faceIt.numTriangles(numTris);
        for(size_t i = 0; i < numTris; ++i)
        {
            trianglePoints.clear();
            triangleVertexIndices.clear();
            faceIt.getTriangle(i, trianglePoints, triangleVertexIndices);

            int triangleVertexOffset[3] = {-1, -1, -1};
            for(size_t j = 0, je = faceVertexIndices.length(); j < je; ++j)
            {
                if(faceVertexIndices[j] == triangleVertexIndices[0])
                    triangleVertexOffset[0] = j;
                else if(faceVertexIndices[j] == triangleVertexIndices[1])
                    triangleVertexOffset[1] = j;
                else if(faceVertexIndices[j] == triangleVertexIndices[2])
                    triangleVertexOffset[2] = j;
            }

            // Reverse the direction of the triangle.
            std::swap(triangleVertexOffset[0], triangleVertexOffset[2]);

            asr::Triangle triangle(
                faceVertexIndices[triangleVertexOffset[0]],
                faceVertexIndices[triangleVertexOffset[1]],
                faceVertexIndices[triangleVertexOffset[2]],
                materialIndex);

            if(m_exportUVs)
            {
                triangle.m_a0 = faceUVIndices[triangleVertexOffset[0]];
                triangle.m_a1 = faceUVIndices[triangleVertexOffset[1]];
                triangle.m_a2 = faceUVIndices[triangleVertexOffset[2]];
            }

            if(m_exportNormals)
            {
                triangle.m_n0 = faceNormalIndices[triangleVertexOffset[0]];
                triangle.m_n1 = faceNormalIndices[triangleVertexOffset[1]];
                triangle.m_n2 = faceNormalIndices[triangleVertexOffset[2]];
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
        for(size_t i = 0, e = meshFn.numVertices(); i < e; ++i, p += 3)
            m_mesh->push_vertex(asr::GVector3(p[0], p[1], p[2]));
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
        const float *p = meshFn.getRawNormals(&status);

        for(size_t i = 0, e = meshFn.numNormals(); i < e; ++i, p += 3)
        {
            asr::GVector3 n(p[0], p[1], p[2]);
            m_mesh->push_vertex_normal(asf::safe_normalize(n));
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
        for(size_t i = 0, e = meshFn.numVertices(); i < e; ++i, p += 3)
        {
            m_mesh->set_vertex_pose(
                i,
                m_shapeExportStep - 1,
                asr::GVector3(p[0], p[1], p[2]));
        }
    }

    if(m_exportNormals)
    {
        m_mesh->reserve_vertex_normals(meshFn.numNormals());
        const float *p = meshFn.getRawNormals(&status);

        for(size_t i = 0, e = meshFn.numNormals(); i < e; ++i, p += 3)
        {
            asr::GVector3 n(p[0], p[1], p[2]);
            m_mesh->set_vertex_normal_pose(
                i,
                m_shapeExportStep - 1,
                asf::safe_normalize(n));
        }
    }
}
