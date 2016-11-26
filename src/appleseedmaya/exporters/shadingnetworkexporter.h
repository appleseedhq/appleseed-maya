
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

#ifndef APPLESEED_MAYA_EXPORTERS_SHADING_NETWORK_EXPORTER_H
#define APPLESEED_MAYA_EXPORTERS_SHADING_NETWORK_EXPORTER_H

// Standard headers.
#include <set>

// Maya headers.
#include <maya/MPlug.h>

// appleseed.renderer headers.
#include "renderer/api/shadergroup.h"

// appleseed.maya headers.
#include "appleseedmaya/exporters/mpxnodeexporter.h"
#include "appleseedmaya/shadingnoderegistry.h"
#include "appleseedmaya/utils.h"


class ShadingNetworkExporter
  : public MPxNodeExporter
{
  public:

    enum Context
    {
        SurfaceContext,
        DisplacementContext
    };

    MString shaderGroupName() const;

    virtual void createEntity(const AppleseedSession::Options& options);

    virtual void flushEntity();

  private:
    friend class ShadingEngineExporter;

    ShadingNetworkExporter(
      const Context                 context,
      const MObject&                object,
      renderer::Project&            project,
      AppleseedSession::SessionMode sessionMode);

    void exportShader(const MObject& node);

    void exportAttributeValue(
      const MPlug&          plug,
      const OSLParamInfo&   paramInfo,
      renderer::ParamArray& shaderParams);

    void exportArrayAttributeValue(
      const MPlug&          plug,
      const OSLParamInfo&   paramInfo,
      renderer::ParamArray& shaderParams);

    void exportConnections(const MObject& node);

    void createCompoundChildConnectionAdapterShader(
      const MPlug&          plug,
      const OSLParamInfo&   paramInfo);

    Context                                   m_context;
    AppleseedEntityPtr<renderer::ShaderGroup> m_shaderGroup;
    std::set<MString, MStringCompareLess>     m_shadersExported;
    size_t                                    m_numAdaptersCreated;
};

typedef boost::shared_ptr<ShadingNetworkExporter> ShadingNetworkExporterPtr;

#endif  // !APPLESEED_MAYA_EXPORTERS_SHADING_ENGINE_EXPORTER_H
