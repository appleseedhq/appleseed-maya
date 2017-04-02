
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

#ifndef APPLESEED_MAYA_TYPEIDS_H
#define APPLESEED_MAYA_TYPEIDS_H

//
// Maya typeids assigned to appleseedhq.
// Everytime a new appleseed Maya node is added, the enum should be updated.
//

enum AppleseedMayaTypeIds
{
    FirstTypeId                     = 0x001279c0,   // 1210816

    RenderGlobalsNodeTypeId         = FirstTypeId,  // 1210816
    SkyDomeLightNodeTypeId          = 0x001279c1,   // 1210817
    PhysicalSkyLightNodeTypeId      = 0x001279c2,   // 1210818
    AsDisneyMaterialNodeTypeId      = 0x001279c3,   // 1210819
    AsGlassMaterialNodeTypeId       = 0x001279c4,   // 1210820
    AsCarPaintMaterialNodeTypeId    = 0x001279c5,   // 1210821
    AsVoronoiNodeTypeId             = 0x001279c6,   // 1210822
    AsColorTransformNodeTypeId      = 0x001279c7,   // 1210823
    AsProjectionNodeTypeId          = 0x001279c8,   // 1210824
    AsNoiseTypeId                   = 0x001279c9,   // 1210825

    LastTypeId                      = 0x00127a3f    // 1210943
};

#endif  // !APPLESEED_MAYA_TYPEIDS_H
