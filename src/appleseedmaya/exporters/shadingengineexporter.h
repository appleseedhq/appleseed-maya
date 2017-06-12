
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

#ifndef APPLESEED_MAYA_EXPORTERS_SHADINGENGINEEXPORTER_H
#define APPLESEED_MAYA_EXPORTERS_SHADINGENGINEEXPORTER_H

// Forward declaration header.
#include "shadingengineexporterfwd.h"

// Maya headers.
#include <maya/MObject.h>
#include "appleseedmaya/_endmayaheaders.h"

// appleseed.renderer headers.
#include "renderer/api/material.h"
#include "renderer/api/surfaceshader.h"

// appleseed.foundation headers.
#include "foundation/core/concepts/noncopyable.h"

// appleseed-maya headers.
#include "appleseedmaya/appleseedsession.h"
#include "appleseedmaya/exporters/shadingnetworkexporterfwd.h"
#include "appleseedmaya/utils.h"

// Forward declarations.
namespace renderer { class Assembly; }

class ShadingEngineExporter
  : public foundation::NonCopyable
{
  public:
    ~ShadingEngineExporter();

    // Create any extra exporter needed by this exporter (shading networks, ...).
    void createExporters(const AppleseedSession::Services& services);

    // Create appleseed entities.
    void createEntities(const AppleseedSession::Options& options);

    // Flush entities to the renderer.
    void flushEntities();

  private:
    friend class NodeExporterFactory;

    ShadingEngineExporter(
        const MObject&                object,
        renderer::Assembly&           mainAssembly,
        AppleseedSession::SessionMode sessionMode);

    AppleseedSession::SessionMode                   m_sessionMode;
    MObject                                         m_object;
    renderer::Assembly&                             m_mainAssembly;
    AppleseedEntityPtr<renderer::Material>          m_material;
    AppleseedEntityPtr<renderer::SurfaceShader>     m_surfaceShader;
    ShadingNetworkExporterPtr                       m_surfaceNetworkExporter;
};

#endif  // !APPLESEED_MAYA_EXPORTERS_SHADINGENGINEEXPORTER_H
