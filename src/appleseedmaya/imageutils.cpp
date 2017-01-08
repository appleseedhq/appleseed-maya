
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2017 Esteban Tovagliari, The appleseedhq Organization
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
#include "appleseedmaya/imageutils.h"

// Maya headers.
#include <maya/MImage.h>

// appleseed.foundation headers.
#include "foundation/image/image.h"
#include "foundation/image/tile.h"

namespace asf = foundation;

namespace ImageUtils
{

void copySwatchImage(const asf::Image& srcImage, MImage& dstImage)
{
    const asf::CanvasProperties& props = srcImage.properties();

    unsigned int width;
    unsigned int height;
    dstImage.getSize(width, height);

    assert(props.m_canvas_width == width);
    assert(props.m_canvas_height == height);
    // ...

    for (size_t ty = 0; ty < props.m_tile_count_y; ++ty)
    {
        for (size_t tx = 0; tx < props.m_tile_count_x; ++tx)
        {
            // todo: do we need to flip the image vertically
            // like in the render view?
            const size_t x0 = props.m_tile_width * tx;
            const size_t y0 = props.m_tile_height * ty;

            const asf::Tile& tile = srcImage.tile(tx, ty);
            const uint8_t *src = tile.get_storage();

            for (size_t j = 0, je = tile.get_height(); j < je; ++j)
            {
                // For swatches, we assume 4 8 bit channels.
                const size_t y = y0 + j;
                uint8_t *dst = dstImage.pixels() + (y * width * 4) + (x0 * 4);

                for (size_t i = 0, ie = tile.get_width(); i < ie; ++i)
                {
                    // Maya docs say RGBA, but it is actually BGRA?.
                    *dst++ = src[2];
                    *dst++ = src[1];
                    *dst++ = src[0];
                    *dst++ = src[3];
                    src += 4;
                }
            }
        }
    }
}

} // namespace ImageUtils.
