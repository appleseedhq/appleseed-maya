
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
#include "meshexporter.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exporters/alphamapexporter.h"
#include "appleseedmaya/exporters/exporterfactory.h"
#include "appleseedmaya/logger.h"

// appleseed.foundation headers.
#include "foundation/utility/string.h"

// Maya headers.
#include <maya/MFloatPointArray.h>
#include <maya/MFnMesh.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MPointArray.h>
#include "appleseedmaya/_endmayaheaders.h"

// Boost headers.
#include "boost/filesystem/convenience.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

namespace bfs = boost::filesystem;
namespace asf = foundation;
namespace asr = renderer;

namespace
{
    void staticMeshObjectHash(const asr::MeshObject& mesh, MurmurHash& hash)
    {
        hash.append(mesh.get_tex_coords_count());
        for(size_t i = 0, e = mesh.get_tex_coords_count(); i < e; ++i)
            hash.append(mesh.get_tex_coords(i));

        hash.append(mesh.get_triangle_count());
        for(size_t i = 0, e = mesh.get_triangle_count(); i < e; ++i)
            hash.append(mesh.get_triangle(i));

        hash.append(mesh.get_material_slot_count());
        for(size_t i = 0, e = mesh.get_material_slot_count(); i < e; ++i)
            hash.append(mesh.get_material_slot(i));

        hash.append(mesh.get_vertex_count());
        for(size_t i = 0, e = mesh.get_vertex_count(); i < e; ++i)
            hash.append(mesh.get_vertex(i));

        hash.append(mesh.get_vertex_normal_count());
        for(size_t i = 0, e = mesh.get_vertex_normal_count(); i < e; ++i)
            hash.append(mesh.get_vertex_normal(i));

        hash.append(mesh.get_vertex_tangent_count());
        for(size_t i = 0, e = mesh.get_vertex_tangent_count(); i < e; ++i)
            hash.append(mesh.get_vertex_tangent(i));
    }

    void meshObjectHash(
        const asr::MeshObject&                  mesh,
        const asf::StringDictionary&            frontMaterialMappings,
        const asf::StringDictionary&            backMaterialMappings,
        MurmurHash&                             hash)
    {
        staticMeshObjectHash(mesh, hash);

        hash.append(mesh.get_motion_segment_count());
        for(size_t j = 0, je = mesh.get_motion_segment_count(); j < je; ++j)
        {
            for(size_t i = 0, e = mesh.get_vertex_count(); i < e; ++i)
                mesh.get_vertex_pose(i, j);

            for(size_t i = 0, e = mesh.get_vertex_normal_count(); i < e; ++i)
                mesh.get_vertex_normal_pose(i, j);

            for(size_t i = 0, e = mesh.get_vertex_tangent_count(); i < e; ++i)
                mesh.get_vertex_tangent_pose(i, j);
        }

        hash.append(mesh.get_parameters());
        hash.append(frontMaterialMappings);
        hash.append(backMaterialMappings);
    }
}

void MeshExporter::registerExporter()
{
    NodeExporterFactory::registerDagNodeExporter("mesh", &MeshExporter::create);
}

DagNodeExporter* MeshExporter::create(
    const MDagPath&                             path,
    asr::Project&                               project,
    AppleseedSession::SessionMode               sessionMode)
{
    if (areObjectAndParentsRenderable(path) == false)
        return nullptr;

    return new MeshExporter(path, project, sessionMode);
}

MeshExporter::MeshExporter(
    const MDagPath&                             path,
    asr::Project&                               project,
    AppleseedSession::SessionMode               sessionMode)
  : ShapeExporter(path, project, sessionMode)
{
}

MeshExporter::~MeshExporter()
{
    if (sessionMode() == AppleseedSession::ProgressiveRenderSession)
    {
        if (m_objectAssembly.get() == 0)
            mainAssembly().objects().remove(m_mesh.get());
    }
}

void MeshExporter::createExporters(const AppleseedSession::Services& services)
{
    const int instanceNumber = dagPath().isInstanced() ? dagPath().instanceNumber() : 0;

    MStatus status;
    MFnDependencyNode depNodeFn(dagPath().node(), &status);
    MPlug plug = depNodeFn.findPlug("instObjGroups");
    plug = plug.elementByLogicalIndex(instanceNumber);

    if (plug.isConnected())
    {
        // We have only one material for the mesh.
        MPlugArray connections;
        plug.connectedTo(connections, false, true);
        MObject shadingEngine = connections[0].node();
        services.createShadingEngineExporter(shadingEngine);
        depNodeFn.setObject(shadingEngine);
        MString materialName = depNodeFn.name() + MString("_material");
        m_frontMaterialMappings.insert("default", materialName.asChar());

        bool doubleSided = false;
        AttributeUtils::get(depNodeFn, "asDoubleSided", doubleSided);
        if (doubleSided)
            m_backMaterialMappings.insert("default", materialName.asChar());
    }
    else
    {
        // The mesh has per-face materials.
        MFnMesh fnMesh(dagPath().node());
        MObjectArray shadingEngines;
        fnMesh.getConnectedShaders(instanceNumber, shadingEngines, m_perFaceAssignments);

        for(unsigned int i = 0, e = shadingEngines.length(); i < e; ++i)
        {
            services.createShadingEngineExporter(shadingEngines[i]);
            depNodeFn.setObject(shadingEngines[i]);
            MString materialName = depNodeFn.name() + MString("_material");

            bool doubleSided = false;
            AttributeUtils::get(depNodeFn, "asDoubleSided", doubleSided);

            if (i == 0)
            {
                m_frontMaterialMappings.insert("default", materialName.asChar());

                if (doubleSided)
                    m_backMaterialMappings.insert("default", materialName.asChar());
            }
            else
            {
                std::string slotName = asf::get_numbered_string("slot#", i);
                m_frontMaterialMappings.insert(slotName.c_str(), materialName.asChar());

                if (doubleSided)
                    m_backMaterialMappings.insert(slotName.c_str(), materialName.asChar());
            }
        }
    }

    if (m_frontMaterialMappings.empty())
    {
        RENDERER_LOG_WARNING(
            "Found mesh %s with no materials.",
            appleseedName().asChar());
    }

    // Create an alpha map exporter if needed.
    depNodeFn.setObject(dagPath().node());
    plug = depNodeFn.findPlug("asAlphaMap", &status);

    MPlugArray connections;
    plug.connectedTo(connections, true, false);

    if (connections.length() != 0)
    {
        MObject alphaMapNode = connections[0].node();
        m_alphaMapExporter = services.createAlphaMapExporter(alphaMapNode);
    }
}

void MeshExporter::createEntities(
    const AppleseedSession::Options&            options,
    const AppleseedSession::MotionBlurTimes&    motionBlurTimes)
{
    shapeAttributesToParams(m_meshParams);
    meshAttributesToParams(m_meshParams);

    MFnMesh meshFn(dagPath());

    m_exportUVs = meshFn.numUVs() != 0;
    if (m_exportUVs)
        AttributeUtils::get(node(), "asExportUVs", m_exportUVs);

    m_exportNormals = meshFn.numNormals() != 0;
    if (m_exportNormals)
        AttributeUtils::get(node(), "asExportNormals", m_exportNormals);

    m_smoothTangents = false;
    if (m_exportUVs)
        AttributeUtils::get(node(), "asSmoothTangents", m_smoothTangents);

    MStatus status;
    MPlug plug = meshFn.findPlug("referenceObject", &status);
    m_exportReference = plug.isConnected();

    if (m_exportReference)
    {
        RENDERER_LOG_INFO(
            "Found reference geometry for mesh %s.",
            m_mesh->get_name());

        // We don't support PRef and NRef yet...
        m_exportReference = false;
    }

    m_numMeshKeys = motionBlurTimes.m_deformTimes.size();
    m_isDeforming = (m_numMeshKeys > 1) && isAnimated(node());
    m_shapeExportStep = 0;

    if (sessionMode() != AppleseedSession::ExportSession)
    {
        MString objectName = appleseedName();
        m_mesh = asr::MeshObjectFactory::create(objectName.asChar(), m_meshParams);
        createMaterialSlots();
        fillTopology();
        exportGeometry();
    }
}

void MeshExporter::exportShapeMotionStep(float time)
{
    // Do not export extra motion steps for static meshes.
    if (!m_isDeforming && m_shapeExportStep > 0)
        return;

    if (sessionMode() == AppleseedSession::ExportSession)
    {
        MString objectName = appleseedName();
        m_mesh = asr::MeshObjectFactory::create(objectName.asChar(), m_meshParams);
        createMaterialSlots();
        fillTopology();
        exportGeometry();

        // Compute smooth tangents if needed.
        if (m_smoothTangents)
        {
            assert(m_exportUVs);
            asr::compute_smooth_vertex_tangents(*m_mesh);
        }

        MurmurHash meshHash;
        staticMeshObjectHash(*m_mesh, meshHash);

//#define APPLESEED_MAYA_OBJ_MESH_EXPORT
#ifdef APPLESEED_MAYA_OBJ_MESH_EXPORT
        const char* extension = ".obj";
#else
        const char* extension = ".binarymesh";
#endif
        const std::string fileName = std::string("_geometry/") + meshHash.toString() + extension;

        bfs::path projectPath = project().search_paths().get_root_path().c_str();
        bfs::path p = projectPath / fileName;

        // Write a geom file for the object if needed.
        if (!bfs::exists(p))
        {
            if (!asr::MeshObjectWriter::write(*m_mesh, "mesh", p.string().c_str()))
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

        if (m_exportReference && m_shapeExportStep == 0)
        {
            // todo: export PRef and possibly NRef here.
        }
    }
    else
    {
        if (m_exportReference && m_shapeExportStep == 0)
        {
            // todo: export PRef and possibly NRef here.
        }

        // We already exported the first mesh key when
        // we exported the topology.
        if (m_shapeExportStep > 0)
            exportMeshKey();
    }

    m_shapeExportStep++;
}

void MeshExporter::flushEntities()
{
    ShapeExporter::flushEntities();

    MString objectName = appleseedName();

    if (sessionMode() == AppleseedSession::ExportSession)
    {
        assert(!m_fileNames.empty());

        // Replace our MeshObject by one referencing the exported meshes.
        asr::ParamArray params = m_mesh->get_parameters();

        if (m_fileNames.size() == 1)
            params.insert("filename", m_fileNames[0].c_str());
        else
        {
            asf::Dictionary fileNames;
            MString key;

            for(size_t i = 0, e = m_fileNames.size(); i < e; ++i)
            {
                key.set(static_cast<double>(i));
                fileNames.insert(key.asChar(), m_fileNames[i].c_str());
            }

            params.insert("filenames", fileNames);
        }

        m_mesh.reset(asr::MeshObjectFactory().create(m_mesh->get_name(), params));
        objectName += ".mesh";
    }
    else
    {
        // Compute smooth tangents if needed.
        if (m_smoothTangents)
        {
            assert(m_exportUVs);
            asr::compute_smooth_vertex_tangents(*m_mesh);
        }
    }

    // Handle alpha maps.
    if (m_alphaMapExporter)
    {
        m_mesh->get_parameters().insert(
            "alpha_map",
            m_alphaMapExporter->textureInstanceName());
    }

    // Compute the object hash for auto-instancing.
    /*
    if (sessionMode() != AppleseedSession::ProgressiveRenderSession)
    {
        meshObjectHash(
            *m_mesh,
            m_frontMaterialMappings,
            m_backMaterialMappings,
            m_shapeHash);
    }
    */

    RENDERER_LOG_DEBUG("Flushing mesh object %s", m_mesh->get_name());
    if (m_objectAssembly.get())
        m_objectAssembly->objects().insert(m_mesh.releaseAs<asr::Object>());
    else
        mainAssembly().objects().insert(m_mesh.releaseAs<asr::Object>());

    RENDERER_LOG_DEBUG("Flushing object instance %s", m_mesh->get_name());
    createObjectInstance(objectName);
}

void MeshExporter::meshAttributesToParams(renderer::ParamArray& params)
{
    int mediumPriority = 0;
    if (AttributeUtils::get(node(), "asMediumPriority", mediumPriority))
        params.insert("medium_priority", mediumPriority);
}

void MeshExporter::createMaterialSlots()
{
    // Create material slots.
    if (!m_frontMaterialMappings.empty())
    {
        m_mesh->reserve_material_slots(m_frontMaterialMappings.size());
        asf::StringDictionary::const_iterator it(m_frontMaterialMappings.begin());
        asf::StringDictionary::const_iterator e(m_frontMaterialMappings.end());
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
        for(unsigned int i = 0, e = faceVertexIndices.length(); i < e; ++i)
        {
            if (m_exportUVs)
            {
                int uvIndex;
                status = faceIt.getUVIndex(static_cast<int>(i), uvIndex);
                faceUVIndices.append(uvIndex);
            }

            if (m_exportNormals)
            {
                unsigned int normalIndex = faceIt.normalIndex(static_cast<int>(i), &status);
                faceNormalIndices.append(normalIndex);
            }
        }

        // Match the triangle indices to the face indices.
        int numTris;
        faceIt.numTriangles(numTris);
        for(int i = 0; i < numTris; ++i)
        {
            trianglePoints.clear();
            triangleVertexIndices.clear();
            faceIt.getTriangle(i, trianglePoints, triangleVertexIndices);

            int triangleVertexOffset[3] = {-1, -1, -1};
            for(unsigned int j = 0, je = faceVertexIndices.length(); j < je; ++j)
            {
                if (faceVertexIndices[j] == triangleVertexIndices[0])
                    triangleVertexOffset[0] = j;
                else if (faceVertexIndices[j] == triangleVertexIndices[1])
                    triangleVertexOffset[1] = j;
                else if (faceVertexIndices[j] == triangleVertexIndices[2])
                    triangleVertexOffset[2] = j;
            }


            asr::Triangle triangle(
                faceVertexIndices[triangleVertexOffset[0]],
                faceVertexIndices[triangleVertexOffset[1]],
                faceVertexIndices[triangleVertexOffset[2]],
                materialIndex);

            if (m_exportUVs)
            {
                triangle.m_a0 = faceUVIndices[triangleVertexOffset[0]];
                triangle.m_a1 = faceUVIndices[triangleVertexOffset[1]];
                triangle.m_a2 = faceUVIndices[triangleVertexOffset[2]];
            }

            if (m_exportNormals)
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
        const float* p = meshFn.getRawPoints(&status);
        for(size_t i = 0, e = meshFn.numVertices(); i < e; ++i, p += 3)
            m_mesh->push_vertex(asr::GVector3(p[0], p[1], p[2]));
    }

    if (m_exportUVs)
    {
        m_mesh->reserve_tex_coords(meshFn.numUVs());
        MFloatArray u, v;
        status = meshFn.getUVs(u, v);
        for(int i = 0, e = meshFn.numUVs(); i < e; ++i)
            m_mesh->push_tex_coords(asr::GVector2(u[i], v[i]));
    }

    if (m_exportNormals)
    {
        const asr::GVector3 Y(0.0f, 1.0f, 0.0f);
        m_mesh->reserve_vertex_normals(meshFn.numNormals());
        const float* p = meshFn.getRawNormals(&status);

        for(int i = 0, e = meshFn.numNormals(); i < e; ++i, p += 3)
        {
            asr::GVector3 n(p[0], p[1], p[2]);
            m_mesh->push_vertex_normal(asf::safe_normalize(n, Y));
        }
    }
}

void MeshExporter::exportMeshKey()
{
    MStatus status;
    MFnMesh meshFn(dagPath());

    if (m_shapeExportStep == 1)
    {
        assert(m_isDeforming);
        assert(m_numMeshKeys > 1);

        // Reserve number of keys.
        m_mesh->set_motion_segment_count(m_numMeshKeys - 1);
    }

    // Vertices.
    {
        const float* p = meshFn.getRawPoints(&status);
        for(size_t i = 0, e = meshFn.numVertices(); i < e; ++i, p += 3)
        {
            m_mesh->set_vertex_pose(
                i,
                m_shapeExportStep - 1,
                asr::GVector3(p[0], p[1], p[2]));
        }
    }

    if (m_exportNormals)
    {
        const asr::GVector3 Y(0.0f, 1.0f, 0.0f);
        m_mesh->reserve_vertex_normals(meshFn.numNormals());
        const float* p = meshFn.getRawNormals(&status);

        for(size_t i = 0, e = meshFn.numNormals(); i < e; ++i, p += 3)
        {
            asr::GVector3 n(p[0], p[1], p[2]);
            m_mesh->set_vertex_normal_pose(
                i,
                m_shapeExportStep - 1,
                asf::safe_normalize(n, Y));
        }
    }
}
