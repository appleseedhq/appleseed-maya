
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

#ifndef APPLESEED_MAYA_EXPORTERS_MESHEXPORTER_H
#define APPLESEED_MAYA_EXPORTERS_MESHEXPORTER_H

// Standard headers.
#include <string>
#include <vector>

// appleseed.maya headers.
#include "appleseedmaya/exporters/shapeexporter.h"

// appleseed.foundation headers.
#include "renderer/api/material.h"
#include "renderer/api/surfaceshader.h"
#include "foundation/utility/searchpaths.h"

// appleseed.renderer headers.
#include "renderer/api/object.h"
#include "renderer/api/scene.h"


class MeshExporter
  : public ShapeExporter
{
  public:

    static void registerExporter();

    static DagNodeExporter *create(
      const MDagPath&               path,
      renderer::Project&            project,
      AppleseedSession::SessionMode sessionMode);

    virtual void collectDependencyNodesToExport(MObjectArray& nodes);

    virtual void createEntity(const AppleseedSession::Options& options);

    virtual void exportShapeMotionStep(float time);

    virtual void flushEntity();

  private:

    MeshExporter(
      const MDagPath&               path,
      renderer::Project&            project,
      AppleseedSession::SessionMode sessionMode);

    void meshAttributesToParams(renderer::ParamArray& params);

    void fillTopology();
    void exportGeometry();

    AppleseedEntityPtr<renderer::MeshObject>      m_mesh;
    AppleseedEntityPtr<renderer::ObjectInstance>  m_objectInstace;
    bool                                          m_exportUVs;
    bool                                          m_exportNormals;
    bool                                          m_exportTangents;
    std::vector<std::string>                      m_fileNames;
};

#endif  // !APPLESEED_MAYA_EXPORTERS_MESHEXPORTER_H
