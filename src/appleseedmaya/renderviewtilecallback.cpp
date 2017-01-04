
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016-2017 Esteban Tovagliari, The appleseedhq Organization
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

// Boost headers.
#include <boost/shared_array.hpp>

// Maya headers.
#include <maya/MRenderView.h>

// appleseed.foundation headers.
#include "foundation/image/canvasproperties.h"
#include "foundation/image/image.h"
#include "foundation/image/pixel.h"
#include "foundation/image/tile.h"
#include "foundation/math/scalar.h"

// appleseed.renderer headers.
#include "renderer/api/frame.h"
#include "renderer/api/log.h"

// appleseed.maya headers.
#include "appleseedmaya/idlejobqueue.h"
#include "appleseedmaya/utils.h"

namespace asf = foundation;
namespace asr = renderer;

namespace
{

const int MaxHighlightSize = 8;

class RenderViewTileCallback
  : public renderer::ITileCallback
{
  public:
    RenderViewTileCallback(int width, int height, RendererController& rendererController, MComputation& computation)
      : m_width(width)
      , m_height(height)
      , m_rendererController(rendererController)
      , m_computation(computation)
    {
        assert(m_width > 0);
        assert(m_height > 0);

        for(int i = 0; i < MaxHighlightSize; ++i)
            m_highlightPixels[i].r = m_highlightPixels[i].g = m_highlightPixels[i].b = m_highlightPixels[i].a = 1.0f;

    }

    virtual void release()
    {
        delete this;
    }

    virtual void pre_render(
        const size_t            x,
        const size_t            y,
        const size_t            width,
        const size_t            height)
    {
        int xmin = static_cast<int>(x);
        int xmax = static_cast<int>(x + width - 1);
        int ymin = static_cast<int>(m_height - y - height);
        int ymax = static_cast<int>(m_height - y - 1);

        int halfWidth  = (xmax - xmin) / 2;
        int halfHeight = (ymax - ymin) / 2;
        int lineSize = std::min(std::min(halfWidth, halfHeight), MaxHighlightSize);

        HighlightTile h(xmin, ymin, xmax, ymax, lineSize, m_highlightPixels, m_rendererController, m_computation);
        IdleJobQueue::pushJob(h);
    }

    virtual void post_render(
        const renderer::Frame*  frame)
    {
        const asf::CanvasProperties& frame_props = frame->image().properties();

        for( size_t ty = 0; ty < frame_props.m_tile_count_y; ++ty )
            for( size_t tx = 0; tx < frame_props.m_tile_count_x; ++tx )
                write_tile(frame, tx, ty);
    }

    virtual void post_render_tile(
        const renderer::Frame*  frame,
        const size_t            tile_x,
        const size_t            tile_y)
    {
        write_tile(frame, tile_x, tile_y);
    }

  private:
    struct HighlightTile
    {
        HighlightTile(
            int                 xmin,
            int                 ymin,
            int                 xmax,
            int                 ymax,
            int                 lineSize,
            RV_PIXEL*           highlightPixels,
            RendererController& rendererController,
            MComputation&       computation)
          : m_xmin(xmin)
          , m_ymin(ymin)
          , m_xmax(xmax)
          , m_ymax(ymax)
          , m_lineSize(lineSize)
          , m_pixels(highlightPixels)
          , m_rendererController(rendererController)
          , m_computation(computation)
        {
        }

        void operator()()
        {
            if (m_computation.isInterruptRequested())
            {
                m_rendererController.set_status(RendererController::AbortRendering);
                return;
            }

            draw_hline(m_xmin, m_xmin + m_lineSize, m_ymin, m_pixels);
            draw_hline(m_xmax - m_lineSize, m_xmax, m_ymin, m_pixels);
            draw_hline(m_xmin, m_xmin + m_lineSize, m_ymax, m_pixels);
            draw_hline(m_xmax - m_lineSize, m_xmax, m_ymax, m_pixels);

            draw_vline(m_xmin, m_ymin, m_ymin + m_lineSize, m_pixels);
            draw_vline(m_xmin, m_ymax - m_lineSize, m_ymax, m_pixels);
            draw_vline(m_xmax, m_ymin, m_ymin + m_lineSize, m_pixels);
            draw_vline(m_xmax, m_ymax - m_lineSize, m_ymax, m_pixels);
        }

        void draw_hline(int x0, int x1, int y, RV_PIXEL *pixels) const
        {
            MRenderView::updatePixels(x0, x1, y, y, pixels, true);
            MRenderView::refresh(x0, x1, y, y);
        }

        void draw_vline(int x, int y0, int y1, RV_PIXEL *pixels) const
        {
            MRenderView::updatePixels(x, x, y0, y1, pixels, true);
            MRenderView::refresh(x, x, y0, y1);
        }

        int                 m_xmin;
        int                 m_ymin;
        int                 m_xmax;
        int                 m_ymax;
        int                 m_lineSize;
        RV_PIXEL*           m_pixels;
        RendererController& m_rendererController;
        MComputation&       m_computation;
    };

    struct WriteTileToRenderView
    {
        WriteTileToRenderView(
            int                             xmin,
            int                             ymin,
            int                             xmax,
            int                             ymax,
            boost::shared_array<RV_PIXEL>   pixels,
            RendererController&             rendererController,
            MComputation&                   computation)
          : m_xmin(xmin)
          , m_ymin(ymin)
          , m_xmax(xmax)
          , m_ymax(ymax)
          , m_pixels(pixels)
          , m_rendererController(rendererController)
          , m_computation(computation)
        {
        }

        void operator()()
        {
            if (m_computation.isInterruptRequested())
            {
                m_rendererController.set_status(RendererController::AbortRendering);
                return;
            }

            MRenderView::updatePixels(m_xmin, m_xmax, m_ymin, m_ymax, m_pixels.get(), true);
            MRenderView::refresh(m_xmin, m_xmax, m_ymin, m_ymax);
        }

        int                             m_xmin;
        int                             m_ymin;
        int                             m_xmax;
        int                             m_ymax;
        boost::shared_array<RV_PIXEL>   m_pixels;
        RendererController&             m_rendererController;
        MComputation&                   m_computation;
    };

    void write_tile(
        const renderer::Frame*  frame,
        const size_t            tile_x,
        const size_t            tile_y)
    {
        const asf::CanvasProperties& frameProps = frame->image().properties();

        const foundation::Tile& tile = frame->image().tile(tile_x, tile_y);
        assert(tile.get_pixel_format() == foundation::PixelFormatFloat);
        assert(tile.get_channel_count() == 4);

        const size_t tileWidth = tile.get_width();
        const size_t tileHeight = tile.get_height();

        boost::shared_array<RV_PIXEL> pixels(new RV_PIXEL[tileWidth * tileHeight]);
        RV_PIXEL *p = pixels.get();

        // Copy and flip the tile verticaly (Maya's renderview is y up).
        for (size_t y = 0; y < tileHeight; y++)
        {
            for (size_t x = 0; x < tileWidth; x++)
            {
                const float* source = reinterpret_cast<const float*>(tile.pixel(x, tileHeight - y - 1));
                p->r = source[0];
                p->g = source[1];
                p->b = source[2];
                p->a = source[3];
                ++p;
            }
        }

        const size_t x = tile_x * frameProps.m_tile_width;
        const size_t y = tile_y * frameProps.m_tile_height;
        int xmin = static_cast<int>(x);
        int xmax = static_cast<int>(x + tileWidth - 1);
        int ymin = static_cast<int>(m_height - y - tileHeight);
        int ymax = static_cast<int>(m_height - y - 1);

        WriteTileToRenderView w(xmin, ymin, xmax, ymax, pixels, m_rendererController, m_computation);
        IdleJobQueue::pushJob(w);
    }

    RV_PIXEL            m_highlightPixels[MaxHighlightSize];
    int                 m_width;
    int                 m_height;
    RendererController& m_rendererController;
    MComputation&       m_computation;
};

} // unnamed.

RenderViewTileCallbackFactory::RenderViewTileCallbackFactory(
    RendererController&  rendererController,
    MComputation&        computation)
  : m_rendererController(rendererController)
  , m_computation(computation)
  , m_width(-1)
  , m_height(-1)
{
}

RenderViewTileCallbackFactory::~RenderViewTileCallbackFactory()
{
    MRenderView::endRender();
}

void RenderViewTileCallbackFactory::release()
{
    delete this;
}

renderer::ITileCallback* RenderViewTileCallbackFactory::create()
{
    return new RenderViewTileCallback(m_width, m_height, m_rendererController, m_computation);
}

void RenderViewTileCallbackFactory::renderViewStart(const renderer::Frame& frame)
{
    const asf::CanvasProperties& frameProps = frame.image().properties();

    m_width = frameProps.m_canvas_width;
    m_height = frameProps.m_canvas_height;

    if (frame.has_crop_window())
    {
        MRenderView::startRegionRender(
            m_width,
            m_height,
            frame.get_crop_window().min.x,
            frame.get_crop_window().max.x,
            frame.get_crop_window().min.y,
            frame.get_crop_window().max.y,
            false,
            true);
    }
    else
    {
        MRenderView::startRender(
            m_width,
            m_height,
            false,
            true);
    }
}
