
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2017-2019 Esteban Tovagliari, The appleseedhq Organization
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
#include "alphamapexporter.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/project.h"
#include "renderer/api/scene.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MFnDependencyNode.h>
#include "appleseedmaya/_endmayaheaders.h"

namespace asf = foundation;
namespace asr = renderer;

AlphaMapExporter* AlphaMapExporter::create(
    const MObject&                  object,
    asr::Project&                   project,
    AppleseedSession::SessionMode   sessionMode)
{
    MString map;
    AttributeUtils::get(object, "map", map);

    // Ignore alpha maps without image files.
    if (map.length() == 0)
        return nullptr;

    return new AlphaMapExporter(object, project, sessionMode);
}

AlphaMapExporter::AlphaMapExporter(
    const MObject&                  object,
    asr::Project&                   project,
    AppleseedSession::SessionMode   sessionMode)
  : m_object(object)
  , m_sessionMode(sessionMode)
  , m_project(project)
  , m_mainAssembly(*project.get_scene()->assemblies().get_by_name("assembly"))
{
}

AlphaMapExporter::~AlphaMapExporter()
{
    if (m_sessionMode == AppleseedSession::ProgressiveRenderSession)
    {
        m_mainAssembly.texture_instances().remove(m_textureInstance.get());
        m_mainAssembly.textures().remove(m_texture.get());
    }
}

void AlphaMapExporter::createEntities()
{
    MFnDependencyNode depNodeFn(m_object);

    MString map;
    AttributeUtils::get(depNodeFn, "map", map);

    MString textureName = depNodeFn.name() + "_texture";
    m_texture = asr::DiskTexture2dFactory().create(
        textureName.asChar(),
        asr::ParamArray()
            .insert("filename", map.asChar())
            .insert("color_space", "linear_rgb"),
        m_project.search_paths());

    MString textureInstanceName = textureName + "_instance";
    m_textureInstance = asr::TextureInstanceFactory().create(
        textureInstanceName.asChar(),
        asr::ParamArray()
            .insert("alpha_mode", "detect")
            .insert("addressing_mode", "clamp")
            .insert("filtering_mode", "bilinear"),
        textureName.asChar());
}

void AlphaMapExporter::flushEntities()
{
    m_mainAssembly.textures().insert(m_texture.release());
    m_mainAssembly.texture_instances().insert(m_textureInstance.release());
}

const char*AlphaMapExporter::textureInstanceName() const
{
    return m_textureInstance->get_name();
}
