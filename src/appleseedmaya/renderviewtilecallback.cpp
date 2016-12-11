
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
#include "renderviewtilecallback.h"

// Standard headers.
#include <cassert>

// appleseed.foundation headers.
#include "foundation/image/canvasproperties.h"
#include "foundation/image/image.h"
#include "foundation/image/pixel.h"
#include "foundation/image/tile.h"
#include "foundation/math/scalar.h"

// appleseed.renderer headers.
#include "renderer/api/frame.h"
#include "renderer/api/log.h"

namespace asf = foundation;
namespace asr = renderer;

RenderViewTileCallback::RenderViewTileCallback()
  : m_renderedPixels(0)
{
}

void RenderViewTileCallback::release()
{
    // We don't need to do anything here.
    // The tile callback factory deletes this instance.
}

void RenderViewTileCallback::pre_render(
    const size_t            x,
    const size_t            y,
    const size_t            width,
    const size_t            height)
{
}

void RenderViewTileCallback::post_render(const renderer::Frame* frame)
{
}

void RenderViewTileCallback::post_render_tile(
    const renderer::Frame*  frame,
    const size_t            tile_x,
    const size_t            tile_y)
{
    const size_t totalPixels = frame->image().properties().m_pixel_count;

    const asf::Tile& tile = frame->image().tile(tile_x, tile_y);
    m_renderedPixels += tile.get_pixel_count();

    // todo: log progress here...

    // Reset progress when rendering is finished for multi-pass renders.
    if( m_renderedPixels == totalPixels )
        m_renderedPixels = 0;
}

RenderViewTileCallbackFactory::RenderViewTileCallbackFactory()
{
    m_callback = new RenderViewTileCallback();
}

RenderViewTileCallbackFactory::~RenderViewTileCallbackFactory()
{
    m_callback->release();
}

void RenderViewTileCallbackFactory::release()
{
    delete this;
}

renderer::ITileCallback* RenderViewTileCallbackFactory::create()
{
    return m_callback;
}
