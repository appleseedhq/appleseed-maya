
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

#ifndef APPLESEED_MAYA_EXPORTERS_ENVLIGHTEXPORTER_H
#define APPLESEED_MAYA_EXPORTERS_ENVLIGHTEXPORTER_H

// appleseed-maya headers.
#include "appleseedmaya/exporters/dagnodeexporter.h"

// Build options header.
#include "renderer/api/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/environmentedf.h"
#include "renderer/api/environmentshader.h"
#include "renderer/api/texture.h"
#include "renderer/api/light.h"

class EnvLightExporter
  : public DagNodeExporter
{
  public:
    ~EnvLightExporter() override;
    void flushEntities() override;

  protected:
    EnvLightExporter(
      const MDagPath&                                   path,
      renderer::Project&                                project,
      AppleseedSession::SessionMode                     sessionMode);

    void createEntities(
        const AppleseedSession::Options&                options,
        const AppleseedSession::MotionBlurSampleTimes&  motionBlurSampleTimes) override;

    AppleseedEntityPtr<renderer::EnvironmentEDF>    m_envLight;
    AppleseedEntityPtr<renderer::EnvironmentShader> m_envShader;
};

class PhysicalSkyLightExporter
  : public EnvLightExporter
{
  public:
    static void registerExporter();

    static DagNodeExporter* create(
      const MDagPath&                                   path,
      renderer::Project&                                project,
      AppleseedSession::SessionMode                     sessionMode);

    ~PhysicalSkyLightExporter() override;

    void createEntities(
        const AppleseedSession::Options&                options,
        const AppleseedSession::MotionBlurSampleTimes&  motionBlurSampleTimes) override;

    void flushEntities() override;

  private:
    PhysicalSkyLightExporter(
      const MDagPath&                                   path,
      renderer::Project&                                project,
      AppleseedSession::SessionMode                     sessionMode);

    AppleseedEntityPtr<renderer::Light> m_sunLight;
};

class SkyDomeLightExporter
  : public EnvLightExporter
{
  public:
    static void registerExporter();

    static DagNodeExporter* create(
      const MDagPath&                                   path,
      renderer::Project&                                project,
      AppleseedSession::SessionMode                     sessionMode);

    ~SkyDomeLightExporter() override;

    void createEntities(
        const AppleseedSession::Options&                options,
        const AppleseedSession::MotionBlurSampleTimes&  motionBlurSampleTimes) override;

    void exportTransformMotionStep(float time) override;

    void flushEntities() override;

    private:
      SkyDomeLightExporter(
        const MDagPath&                                 path,
        renderer::Project&                              project,
        AppleseedSession::SessionMode                   sessionMode);

      AppleseedEntityPtr<renderer::Texture>         m_mapTexture;
      AppleseedEntityPtr<renderer::TextureInstance> m_mapTextureInstance;
};

#endif  // !APPLESEED_MAYA_EXPORTERS_ENVLIGHTEXPORTER_H
