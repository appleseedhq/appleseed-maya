
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
    AsDisneyNodeTypeId              = 0x001279c3,   // 1210819
    AsGlassNodeTypeId               = 0x001279c4,   // 1210820
    AsCarPaintMaterialNodeTypeId    = 0x001279c5,   // 1210821
    AsVoronoi3DNodeTypeId           = 0x001279c6,   // 1210822
    AsVoronoi2DNodeTypeId           = 0x001279c7,   // 1210823
    AsColorTransformNodeTypeId      = 0x001279c8,   // 1210824
    AsProjectionNodeTypeId          = 0x001279c9,   // 1210825
    AsNoise3DNodeTypeId             = 0x001279ca,   // 1210826
    AsNoise2DNodeTypeId             = 0x001279cb,   // 1210827
    AlphaMapNodeTypeId              = 0x001279cc,   // 1210828
    AsLuminanceNodeTypeId           = 0x001279cd,   // 1210829
    AsColorCorrectNodeTypeId        = 0x001279ce,   // 1210830
    AsColorSpaceNodeTypeId          = 0x001279cf,   // 1210831
    AsBlackbodyNodeTypeId           = 0x001279d0,   // 1210832
    AsBlendColorNodeTypeId          = 0x001279d1,   // 1210833
    AsRandomizeTexturesNodeTypeId   = 0x001279d2,   // 1210834
    AsVaryColorNodeTypeId           = 0x001279d3,   // 1210835
    AsStandardSurfaceNodeTypeId     = 0x001279d4,   // 1210836
    AsLayeredSurfaceNodeTypeId      = 0x001279d5,   // 1210837
    AsSkinNodeTypeId                = 0x001279d6,   // 1210838
    AsPlasticNodeTypeId             = 0x001279d7,   // 1210839
    AsThinFilmNodeTypeId            = 0x001279d8,   // 1210840

    LastTypeId                      = 0x00127a3f    // 1210943
};

#endif  // !APPLESEED_MAYA_TYPEIDS_H
