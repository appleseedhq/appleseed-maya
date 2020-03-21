
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
#include "alphamapexporterfwd.h"

// appleseed-maya headers.
#include "appleseedmaya/appleseedsession.h"
#include "appleseedmaya/utils.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/texture.h"

// appleseed.foundation headers.
#include "foundation/core/concepts/noncopyable.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MObject.h>
#include "appleseedmaya/_endmayaheaders.h"

// Forward declarations.
namespace renderer { class Assembly; }
namespace renderer { class Project; }

class AlphaMapExporter
  : public foundation::NonCopyable
{
  public:
    static AlphaMapExporter* create(
      const MObject&                object,
      renderer::Project&            project,
      AppleseedSession::SessionMode sessionMode);

    // Destructor.
    ~AlphaMapExporter();

    // Create appleseed entities.
    void createEntities();

    // Flush entities to the renderer.
    void flushEntities();

    const char* textureInstanceName() const;

  private:
    AlphaMapExporter(
      const MObject&                object,
      renderer::Project&            project,
      AppleseedSession::SessionMode sessionMode);

    MObject                                         m_object;
    AppleseedSession::SessionMode                   m_sessionMode;
    renderer::Project&                              m_project;
    renderer::Assembly&                             m_mainAssembly;
    AppleseedEntityPtr<renderer::Texture>           m_texture;
    AppleseedEntityPtr<renderer::TextureInstance>   m_textureInstance;
};
