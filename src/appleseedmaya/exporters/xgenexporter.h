
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016-2018 Esteban Tovagliari, The appleseedhq Organization
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
#include "appleseedmaya/exporters/dagnodeexporter.h"

// appleseed.renderer headers.
#include "renderer/api/scene.h"
#include "renderer/api/utility.h"

// Forward declarations.
namespace renderer { class Project; }

class XGenExporter
  : public DagNodeExporter
{
  public:
    static void registerExporter();

    static DagNodeExporter* create(
      const MDagPath&                                   path,
      renderer::Project&                                project,
      AppleseedSession::SessionMode                     sessionMode);

    ~XGenExporter() override;

    void createExporters(const AppleseedSession::IExporterFactory& exporter_factory) override;

    void createEntities(
        const AppleseedSession::Options&                options,
        const AppleseedSession::MotionBlurSampleTimes&  motionBlurSampleTimes) override;

    void exportTransformMotionStep(float time) override;

    void flushEntities() override;

  private:
    XGenExporter(
      const MDagPath&                                   path,
      renderer::Project&                                project,
      AppleseedSession::SessionMode                     sessionMode);

    renderer::TransformSequence                     m_transformSequence;
    AppleseedEntityPtr<renderer::Assembly>          m_assembly;
    AppleseedEntityPtr<renderer::AssemblyInstance>  m_assemblyInstance;

};

