
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
#include "shadingnodeexporterfwd.h"

// appleseed-maya headers.
#include "appleseedmaya/appleseedsession.h"
#include "appleseedmaya/utils.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.foundation headers.
#include "foundation/core/concepts/noncopyable.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MString.h>
#include "appleseedmaya/_endmayaheaders.h"

// Forward declarations.
class OSLParamInfo;
class OSLShaderInfo;
namespace renderer { class ParamArray; }
namespace renderer { class ShaderGroup; }

class ShadingNodeExporter
  : public foundation::NonCopyable
{
  public:
    static void registerExporters();

    static ShadingNodeExporter* create(
        const MObject&                  object,
        renderer::ShaderGroup&          shaderGroup);

    // Destructor.
    virtual ~ShadingNodeExporter();

    // Create appleseed entities.
    void createEntities(ShadingNodeExporterMap& exporters);

    // Flush entities to the renderer.
    void flushEntities();

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

    void exportRampValue(
        const MPlug&                    plug,
        const OSLParamInfo&             paramInfo,
        renderer::ParamArray&           shaderParams) const;

    virtual bool layerAndParamNameFromPlug(
        const MPlug&                    plug,
        MString&                        layerName,
        MString&                        paramName);

    MObject node() const;

    bool hasConnections(const MPlug& plug, const bool asDst, const bool asSrc) const;
    bool hasChildrenConnections(const MPlug& plug, const bool asDst, const bool asSrc) const;
    bool hasElementConnections(const MPlug& plug, const bool asDst, const bool asSrc) const;

    const OSLShaderInfo& getShaderInfo() const;

    ShadingNodeExporter* findExporterForNode(
        ShadingNodeExporterMap&         exporters,
        const MObject&                  node);

    ShadingNodeExporter* getSrcPlugAndExporter(
        const MPlug&                    plug,
        ShadingNodeExporterMap&         exporters,
        MPlug&                          srcPlug);

    MString createAdaptorShader(
        const MString&                  shaderName,
        const MString&                  layerName,
        const renderer::ParamArray&     params);

    void createInputFloatCompoundAdaptorShader(
        const OSLParamInfo&             paramInfo,
        const MPlug&                    plug,
        ShadingNodeExporterMap&         exporters,
        const MString&                  shaderName,
        const MString&                  layerName,
        const char**                    shaderParamNames,
        const char*                     shaderOutputParamName);

    bool createOutputFloatCompoundAdaptorShader(
        const MPlug&                    plug,
        const MString&                  shaderName,
        const char**                    shaderParamNames,
        const char*                     shaderInputParamName,
        MString&                        layerName,
        MString&                        paramName);

    MObject                         m_object;
    renderer::ShaderGroup&          m_shaderGroup;
};

