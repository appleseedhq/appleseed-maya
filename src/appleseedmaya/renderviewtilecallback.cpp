
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

// Boost headers.
#include <boost/shared_array.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

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

class RenderViewTileCallback
  : public renderer::ITileCallback
{
  public:
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
    struct WriteTileToRenderView
    {
        WriteTileToRenderView(
            int                             xmin,
            int                             ymin,
            int                             xmax,
            int                             ymax,
            boost::shared_array<RV_PIXEL>   pixels)
        {
            m_xmin = xmin;
            m_ymin = ymin;
            m_xmax = xmax;
            m_ymax = ymax;
            m_pixels = pixels;
        }

        void operator()()
        {
            std::cout << "write tile to renderview!" << std::endl;
            // todo:
            // MRenderView::refresh(m_xmin, m_xmax, m_ymin, yMax);
            // MRenderView::updatePixels(m_xmin, m_xmax, m_ymax, yMax, m_pixels.get(), true);
        }

        int                             m_xmin;
        int                             m_ymin;
        int                             m_xmax;
        int                             m_ymax;
        boost::shared_array<RV_PIXEL>   m_pixels;
    };

    void write_tile(
        const renderer::Frame*  frame,
        const size_t            tile_x,
        const size_t            tile_y)
    {
        const asf::CanvasProperties& frameProps = frame->image().properties();

        int xmin = tile_x * frameProps.m_tile_width;
        int xmax = xmin + frameProps.m_tile_width;
        int ymin = tile_y * frameProps.m_tile_height;
        int ymax = ymin + frameProps.m_tile_height;

        boost::shared_array<RV_PIXEL> pixels(new RV_PIXEL[(xmax - xmin) * (ymax - ymin)]);

        // todo: copy pixels here...

        WriteTileToRenderView w(xmin, ymin, xmax, xmax, pixels);
        IdleJobQueue::pushJob(w);
    }
};

} // unnamed.

RenderViewTileCallbackFactory::RenderViewTileCallbackFactory()
{
}

RenderViewTileCallbackFactory::~RenderViewTileCallbackFactory()
{
    renderViewEnd();
}

void RenderViewTileCallbackFactory::release()
{
    delete this;
}

renderer::ITileCallback* RenderViewTileCallbackFactory::create()
{
    return new RenderViewTileCallback();
}

void RenderViewTileCallbackFactory::renderViewStart(const renderer::Frame& frame)
{
    const asf::CanvasProperties& frameProps = frame.image().properties();

    if(frame.has_crop_window())
    {
        MRenderView::startRegionRender(
            frameProps.m_canvas_width,
            frameProps.m_canvas_height,
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
            frameProps.m_canvas_width,
            frameProps.m_canvas_height,
            false,
            true);
    }
}

void RenderViewTileCallbackFactory::renderViewEnd()
{
    MRenderView::endRender();
}
