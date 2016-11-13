
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
#include "appleseedmaya/exporters/shadingengineexporter.h"

// appleseed.renderer headers.
#include "renderer/api/material.h"
#include "renderer/api/scene.h"

// appleseed.maya headers.
#include "appleseedmaya/exporters/exporterfactory.h"


namespace asf = foundation;
namespace asr = renderer;

void ShadingEngineExporter::registerExporter()
{
    NodeExporterFactory::registerMPxNodeExporter("shadingEngine", &ShadingEngineExporter::create);
}

MPxNodeExporter *ShadingEngineExporter::create(const MObject& object, asr::Project& project)
{
    return new ShadingEngineExporter(object, project);
}

ShadingEngineExporter::ShadingEngineExporter(const MObject& object, asr::Project& project)
  : MPxNodeExporter(object, project)
{
}

void ShadingEngineExporter::createEntity()
{
    MString name = appleseedName();

    asr::ParamArray surfaceShaderParams;
    m_surfaceShader = asr::PhysicalSurfaceShaderFactory().create(
        name.asChar(),
        surfaceShaderParams);

    asr::ParamArray materialParams;
    materialParams.insert("surface_shader", name.asChar());
    m_material = asr::OSLMaterialFactory().create(name.asChar(), materialParams);
}

void ShadingEngineExporter::flushEntity()
{
    mainAssembly().surface_shaders().insert(m_surfaceShader);
    mainAssembly().materials().insert(m_material);
}
