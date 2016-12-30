
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

#ifndef APPLESEED_MAYA_EXPORTERS_SHADING_NODE_EXPORTER_H
#define APPLESEED_MAYA_EXPORTERS_SHADING_NODE_EXPORTER_H

// Forward declaration header.
#include "shadingnodeexporterfwd.h"

// Standard headers.
#include <vector>

// Maya headers.
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MString.h>

// appleseed.maya headers.
#include "appleseedmaya/appleseedsession.h"
#include "appleseedmaya/shadingnoderegistry.h"
#include "appleseedmaya/utils.h"

// Forward declarations.
namespace renderer { class ShaderGroup; }

class ShadingNodeExporter
  : public NonCopyable
{
  public:

    static void registerExporters();

    static ShadingNodeExporter *create(
        const MObject&          object,
        renderer::ShaderGroup&  shaderGroup);

    // Create appleseed entities.
    void createEntities(
        const ShadingNodeExporterMap&    exporters,
        const AppleseedSession::Options& options);

    // Flush entities to the renderer.
    void flushEntities();

    virtual bool layerAndParamNameFromPlug(
        const MPlug&             plug,
        MString&                 layerName,
        MString&                 paramName) const;

  protected:

    ShadingNodeExporter(
        const MObject&                  object,
        renderer::ShaderGroup&          shaderGroup);

    virtual void exportShaderParameters(
        const OSLShaderInfo&            shaderInfo,
        renderer::ParamArray&           shaderParams) const;

    virtual void exportParameterValue(
        const MPlug&                    plug,
        const OSLParamInfo&             paramInfo,
        renderer::ParamArray&           shaderParams) const;

    void exportValue(
        const MPlug&                    plug,
        const OSLParamInfo&             paramInfo,
        renderer::ParamArray&           shaderParams) const;

    void exportArrayValue(
        const MPlug&                    plug,
        const OSLParamInfo&             paramInfo,
        renderer::ParamArray&           shaderParams) const;

    MObject node() const;

    const OSLShaderInfo& getShaderInfo() const;

    const ShadingNodeExporter *findExporterForNode(
        const ShadingNodeExporterMap&   exporters,
        const MObject&                  node) const;

    struct ShaderEntry
    {
        MString                 m_shaderType;
        MString                 m_shaderName;
        MString                 m_layerName;
        renderer::ParamArray    m_params;
    };

    struct ConnectionEntry
    {
        MString m_srcLayerName;
        MString m_srcParam;
        MString m_dstLayerName;
        MString m_dstParam;
    };

    MObject                         m_object;
    renderer::ShaderGroup&          m_shaderGroup;
    std::vector<ShaderEntry>        m_shaders;
    std::vector<ConnectionEntry>    m_connections;
};

#endif  // !APPLESEED_MAYA_EXPORTERS_SHADING_NODE_EXPORTER_H
