
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

// Forward declaration header.
#include "shadingnetworkexporterfwd.h"

// appleseed-maya headers.
#include "appleseedmaya/appleseedsession.h"
#include "appleseedmaya/exporters/shadingnodeexporterfwd.h"
#include "appleseedmaya/utils.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/shadergroup.h"

// appleseed.foundation headers.
#include "foundation/core/concepts/noncopyable.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MString.h>
#include "appleseedmaya/_endmayaheaders.h"

// Standard headers.
#include <map>
#include <vector>

// Forward declarations.
namespace renderer { class Assembly; }

class ShadingNetworkExporter
  : public foundation::NonCopyable
{
  public:
    ~ShadingNetworkExporter();

    // Return the name of the appleseed shader group created by this exporter.
    MString shaderGroupName() const;

    // Create appleseed entities.
    void createEntities();

    // Flush entities to the renderer.
    void flushEntities();

  private:
    friend class NodeExporterFactory;

    ShadingNetworkExporter(
      const ShadingNetworkContext   context,
      const MObject&                object,
      const MPlug&                  outputPlug,
      renderer::Assembly&           mainAssembly,
      AppleseedSession::SessionMode sessionMode);

    void createShaderNodeExporters(const MObject& node);

    ShadingNetworkContext                       m_context;
    AppleseedSession::SessionMode               m_sessionMode;
    MObject                                     m_object;
    MPlug                                       m_outputPlug;
    renderer::Assembly&                         m_mainAssembly;
    AppleseedEntityPtr<renderer::ShaderGroup>   m_shaderGroup;
    std::vector<ShadingNodeExporterPtr>         m_nodeExporters;
    ShadingNodeExporterMap                      m_namesToExporters;
};

