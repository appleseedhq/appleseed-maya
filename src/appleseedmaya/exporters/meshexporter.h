
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016-2019 Esteban Tovagliari, The appleseedhq Organization
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

#pragma once

// appleseed-maya headers.
#include "appleseedmaya/exporters/alphamapexporterfwd.h"
#include "appleseedmaya/exporters/shapeexporter.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/material.h"
#include "renderer/api/object.h"
#include "renderer/api/scene.h"
#include "renderer/api/surfaceshader.h"

// appleseed.foundation headers.
#include "foundation/utility/searchpaths.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MIntArray.h>
#include "appleseedmaya/_endmayaheaders.h"

// Standard headers.
#include <string>
#include <vector>

class MeshExporter
  : public ShapeExporter
{
  public:
    static void registerExporter();

    static DagNodeExporter* create(
      const MDagPath&                                   path,
      renderer::Project&                                project,
      AppleseedSession::SessionMode                     sessionMode);

    ~MeshExporter() override;

    void createExporters(const AppleseedSession::IExporterFactory& exporter_factory) override;

    void createEntities(
        const AppleseedSession::Options&                options,
        const AppleseedSession::MotionBlurSampleTimes&  motionBlurSampleTimes) override;

    void exportShapeMotionStep(float time) override;

    void flushEntities() override;

    bool supportsInstancing() const override;

    MurmurHash hash() const override;

  private:
    MeshExporter(
      const MDagPath&                                   path,
      renderer::Project&                                project,
      AppleseedSession::SessionMode                     sessionMode);

    void meshAttributesToParams(renderer::ParamArray& params);

    struct MeshAndData
    {
        MObject m_mesh;
        MObject m_data;
    };

    int getSmoothLevel(MStatus* ReturnStatus = nullptr) const;
    MeshAndData getFinalMesh(MStatus* ReturnStatus = nullptr) const;

    void createMaterialSlots();
    void fillTopology(MObject mesh);
    void exportGeometry(MObject mesh);
    void exportMeshKey(MObject mesh);

    AppleseedEntityPtr<renderer::MeshObject>    m_mesh;
    renderer::ParamArray                        m_meshParams;
    bool                                        m_exportUVs;
    bool                                        m_exportNormals;
    bool                                        m_smoothTangents;
    std::vector<std::string>                    m_fileNames;
    MIntArray                                   m_perFaceAssignments;
    bool                                        m_isDeforming;
    size_t                                      m_numMeshKeys;
    size_t                                      m_shapeExportStep;
    AlphaMapExporterPtr                         m_alphaMapExporter;
    MurmurHash                                  m_hash;
};

