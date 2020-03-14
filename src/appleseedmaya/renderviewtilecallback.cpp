
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

// Interface header.
#include "renderviewtilecallback.h"

// appleseed-maya headers.
#include "appleseedmaya/idlejobqueue.h"
#include "appleseedmaya/utils.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/frame.h"
#include "renderer/api/log.h"

// appleseed.foundation headers.
#include "foundation/image/canvasproperties.h"
#include "foundation/image/image.h"
#include "foundation/image/pixel.h"
#include "foundation/image/tile.h"
#include "foundation/math/scalar.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MRenderView.h>
#include "appleseedmaya/_endmayaheaders.h"

// Standard headers.
#include <cassert>
#include <cstdint>
#include <memory>

namespace asf = foundation;
namespace asr = renderer;

namespace
{
    const int MaxHighlightSize = 8;

    class RenderViewTileCallback
      : public renderer::TileCallbackBase
    {
      public:
        RenderViewTileCallback(
            const asf::AABB2i&      displayWindow,
            const asf::AABB2i&      dataWindow,
            RendererController&     rendererController,
            ComputationPtr&         computation)
          : m_displayWindow(displayWindow)
          , m_dataWindow(dataWindow)
          , m_rendererController(rendererController)
          , m_computation(computation)
        {
            for (int i = 0; i < MaxHighlightSize; ++i)
            {
                m_highlightPixels[i].r = 1.0f;
                m_highlightPixels[i].g = 1.0f;
                m_highlightPixels[i].b = 1.0f;
                m_highlightPixels[i].a = 1.0f;
            }
        }

        void release() override
        {
            delete this;
        }

        virtual void on_tile_begin(
            const asr::Frame*       frame,
            const size_t            tile_x,
            const size_t            tile_y,
            const size_t            thread_index,
            const size_t            thread_count) override
        {
            // Temporarily disabled.
            /*
            const asf::CanvasProperties& props = frame->image().properties();
            pre_render(
                tile_x * props.m_tile_width,
                tile_y * props.m_tile_height,
                props.m_tile_width,
                props.m_tile_height);
            */
        }

        virtual void on_tile_end(
            const asr::Frame*       frame,
            const size_t            tile_x,
            const size_t            tile_y) override
        {
            write_tile(frame, tile_x, tile_y);
        }

        void on_progressive_frame_update(
            const asr::Frame&       frame,
            const double            time,
            const std::uint64_t     samples,
            const double            samples_per_pixel,
            const std::uint64_t     samples_per_second) override
        {
            const asf::CanvasProperties& props = frame.image().properties();

            for (size_t ty = 0; ty < props.m_tile_count_y; ++ty)
            {
                for (size_t tx = 0; tx < props.m_tile_count_x; ++tx)
                    write_tile(&frame, tx, ty);
            }
        }

      private:
        struct HighlightTile
        {
            HighlightTile(
                const size_t        xmin,
                const size_t        ymin,
                const size_t        xmax,
                const size_t        ymax,
                const size_t        lineSize,
                RV_PIXEL*           highlightPixels,
                RendererController& rendererController,
                ComputationPtr      computation)
              : m_xmin(static_cast<unsigned int>(xmin))
              , m_ymin(static_cast<unsigned int>(ymin))
              , m_xmax(static_cast<unsigned int>(xmax))
              , m_ymax(static_cast<unsigned int>(ymax))
              , m_lineSize(static_cast<unsigned int>(lineSize))
              , m_pixels(highlightPixels)
              , m_rendererController(rendererController)
              , m_computation(computation)
            {
            }

            void operator()()
            {
                if (m_computation && m_computation->isInterruptRequested())
                {
                    m_rendererController.set_status(RendererController::AbortRendering);
                    return;
                }

                draw_hline(m_xmin             , m_xmin + m_lineSize, m_ymin);
                draw_hline(m_xmax - m_lineSize, m_xmax             , m_ymin);
                draw_hline(m_xmin             , m_xmin + m_lineSize, m_ymax);
                draw_hline(m_xmax - m_lineSize, m_xmax             , m_ymax);

                draw_vline(m_xmin, m_ymin             , m_ymin + m_lineSize);
                draw_vline(m_xmin, m_ymax - m_lineSize, m_ymax             );
                draw_vline(m_xmax, m_ymin             , m_ymin + m_lineSize);
                draw_vline(m_xmax, m_ymax - m_lineSize, m_ymax             );

                MRenderView::refresh(m_xmin, m_xmax, m_ymin, m_ymax);
            }

            void draw_hline(
                const unsigned int  x0,
                const unsigned int  x1,
                const unsigned int  y) const
            {
                MRenderView::updatePixels(x0, x1, y, y, m_pixels, true);
            }

            void draw_vline(
                const unsigned int  x,
                const unsigned int  y0,
                const unsigned int  y1) const
            {
                MRenderView::updatePixels(x, x, y0, y1, m_pixels, true);
            }

            const unsigned int  m_xmin;
            const unsigned int  m_ymin;
            const unsigned int  m_xmax;
            const unsigned int  m_ymax;
            const unsigned int  m_lineSize;
            RV_PIXEL*           m_pixels;
            RendererController& m_rendererController;
            ComputationPtr      m_computation;
        };

        struct WriteTileToRenderView
        {
            WriteTileToRenderView(
                const size_t                    xmin,
                const size_t                    ymin,
                const size_t                    xmax,
                const size_t                    ymax,
                std::shared_ptr<RV_PIXEL>       pixels,
                RendererController&             rendererController,
                ComputationPtr                  computation)
              : m_xmin(static_cast<unsigned int>(xmin))
              , m_ymin(static_cast<unsigned int>(ymin))
              , m_xmax(static_cast<unsigned int>(xmax))
              , m_ymax(static_cast<unsigned int>(ymax))
              , m_pixels(pixels)
              , m_rendererController(rendererController)
              , m_computation(computation)
            {
            }

            void operator()()
            {
                if (m_computation && m_computation->isInterruptRequested())
                {
                    m_rendererController.set_status(RendererController::AbortRendering);
                    return;
                }

                MRenderView::updatePixels(m_xmin, m_xmax, m_ymin, m_ymax, m_pixels.get(), true);
                MRenderView::refresh(m_xmin, m_xmax, m_ymin, m_ymax);
            }

            const unsigned int              m_xmin;
            const unsigned int              m_ymin;
            const unsigned int              m_xmax;
            const unsigned int              m_ymax;
            std::shared_ptr<RV_PIXEL>       m_pixels;
            RendererController&             m_rendererController;
            ComputationPtr                  m_computation;
        };

        void pre_render(
            const size_t        x,
            const size_t        y,
            const size_t        width,
            const size_t        height)
        {
            int xmin = static_cast<int>(x);
            int ymin = static_cast<int>(y);
            int xmax = static_cast<int>(x + width  - 1);
            int ymax = static_cast<int>(y + height - 1);

            if (!intersect_with_data_window(xmin, ymin, xmax, ymax))
                return;

            int halfWidth  = (xmax - xmin + 1) / 2;
            int halfHeight = (ymax - ymin + 1) / 2;
            int lineSize = std::min(std::min(halfWidth, halfHeight), MaxHighlightSize - 1);

            // Flip Y interval vertically (Maya is Y up).
            flip_pixel_interval(displayWindowHeight(), ymin, ymax);
            HighlightTile highlightJob(xmin, ymin, xmax, ymax, lineSize, m_highlightPixels, m_rendererController, m_computation);
            IdleJobQueue::pushJob(highlightJob);
        }

        void write_tile(
            const asr::Frame*   frame,
            const size_t        tile_x,
            const size_t        tile_y)
        {
            assert(frame != nullptr);
            const foundation::Tile& tile = frame->image().tile(tile_x, tile_y);
            assert(tile.get_pixel_format() == foundation::PixelFormatFloat);
            assert(tile.get_channel_count() == 4);

            const asf::CanvasProperties& props = frame->image().properties();
            const int x0 = static_cast<int>(tile_x * props.m_tile_width);
            const int y0 = static_cast<int>(tile_y * props.m_tile_height);

            int xmin = x0;
            int ymin = y0;
            int xmax = xmin + static_cast<int>(tile.get_width()) - 1;
            int ymax = ymin + static_cast<int>(tile.get_height()) - 1;

            if (!intersect_with_data_window(xmin, ymin, xmax, ymax))
                return;

            const size_t w = xmax - xmin + 1;
            const size_t h = ymax - ymin + 1;
            std::shared_ptr<RV_PIXEL> pixels(new RV_PIXEL[w * h], ArrayDeleter<RV_PIXEL>());
            RV_PIXEL* p = pixels.get();

            // Copy and flip the tile verticaly (Maya's renderview is y up).
            for (int j = ymax; j >= ymin; --j)
            {
                size_t y = j - y0;

                for (int i = xmin; i <= xmax; ++i)
                {
                    const size_t x = i - x0;
                    p->r = tile.get_component<float>(x, y, 0);
                    p->g = tile.get_component<float>(x, y, 1);
                    p->b = tile.get_component<float>(x, y, 2);
                    p->a = tile.get_component<float>(x, y, 3);
                    p++;
                }
            }

            flip_pixel_interval(displayWindowHeight(), ymin, ymax);
            WriteTileToRenderView tileJob(xmin, ymin, xmax, ymax, pixels, m_rendererController, m_computation);
            IdleJobQueue::pushJob(tileJob);
        }

        int displayWindowHeight() const
        {
            return m_displayWindow.max.y + 1;
        }

        bool intersect_with_data_window(
            int&               xmin,
            int&               ymin,
            int&               xmax,
            int&               ymax) const
        {
            xmin = std::max(m_dataWindow.min.x, xmin);
            xmax = std::min(m_dataWindow.max.x, xmax);
            ymin = std::max(m_dataWindow.min.y, ymin);
            ymax = std::min(m_dataWindow.max.y, ymax);

            if (xmax < xmin || ymax < ymin)
                return false;

            return true;
        }

        RV_PIXEL            m_highlightPixels[MaxHighlightSize];
        const asf::AABB2i   m_displayWindow;
        const asf::AABB2i   m_dataWindow;
        RendererController& m_rendererController;
        ComputationPtr      m_computation;
    };
}

RenderViewTileCallbackFactory::RenderViewTileCallbackFactory(
    RendererController&  rendererController,
    ComputationPtr       computation)
  : m_rendererController(rendererController)
  , m_computation(computation)
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
    return new RenderViewTileCallback(
        m_displayWindow,
        m_dataWindow,
        m_rendererController,
        m_computation);
}

void RenderViewTileCallbackFactory::renderViewStart(const renderer::Frame& frame)
{
    const asf::CanvasProperties& frameProps = frame.image().properties();

    const int width = static_cast<int>(frameProps.m_canvas_width);
    const int height = static_cast<int>(frameProps.m_canvas_height);
    m_displayWindow = asf::AABB2i(asf::Vector2i(0, 0), asf::Vector2i(width - 1, height - 1));

    if (frame.has_crop_window())
    {
        m_dataWindow = frame.get_crop_window();

        int ymin = m_dataWindow.min.y;
        int ymax = m_dataWindow.max.y;
        flip_pixel_interval(height, ymin, ymax);

        MRenderView::startRegionRender(
            static_cast<unsigned int>(width),
            static_cast<unsigned int>(height),
            static_cast<unsigned int>(m_dataWindow.min.x),
            static_cast<unsigned int>(m_dataWindow.max.x),
            static_cast<unsigned int>(ymin),
            static_cast<unsigned int>(ymax),
            false,
            true);
    }
    else
    {
        m_dataWindow = m_displayWindow;
        MRenderView::startRender(width, height, false, true);
    }
}
