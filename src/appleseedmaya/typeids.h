
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016-2018 Esteban Tovagliari, The appleseedhq Organization
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
    AsCarPaintNodeTypeId            = 0x001279c5,   // 1210821
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
    AsIdManifoldNodeTypeId          = 0x001279d9,   // 1210841
    AsSwitchTextureNodeTypeId       = 0x001279da,   // 1210842
    AsDoubleShadeNodeTypeId         = 0x001279db,   // 1210843
    AsRaySwitchNodeTypeId           = 0x001279dc,   // 1210844
    AsBlendShaderNodeTypeId         = 0x001279dd,   // 1210845
    AsLayerTextureNodeTypeId        = 0x001279de,   // 1210846
    AsFlakesNodeTypeId              = 0x001279df,   // 1210847
    AsAnisoVectorFieldNodeTypeId    = 0x001278e0,   // 1210848
    AsCreateMaskNodeTypeId          = 0x001278e1,   // 1210849
    AsRampNodeTypeId                = 0x001278e2,   // 1210850
    AsVolumeRampNodeTypeId          = 0x001278e3,   // 1210851
    AsSubsurfaceNodeTypeId          = 0x001279e4,   // 1210852
    AsTriPlanarNodeTypeId           = 0x001279e5,   // 1210853
    AsDirtNodeTypeId                = 0x001279e6,   // 1210854
    AsCurvatureNodeTypeId           = 0x001279e7,   // 1210855
    AsBevelNodeTypeId               = 0x001279e8,   // 1210856
    AsHairNodeTypeId                = 0x001279e9,   // 1210857
    AsClothNodeTypeId               = 0x001279ea,   // 1210858
    AsVolumeNodeTypeId              = 0x001279eb,   // 1210859
    AsShadowCatcherNodeTypeId       = 0x001279ec,   // 1210860
    AsEmissionNodeTypeId            = 0x001279ed,   // 1210861
    AsMatteNodeTypeId               = 0x001279ee,   // 1210862
    AsGlobalsNodeTypeId             = 0x001279ef,   // 1210863
    AsSpaceTransformNodeTypeId      = 0x001279f0,   // 1210864
    AsFresnelNodeTypeId             = 0x001279f1,   // 1210865
    AsSwizzleNodeTypeId             = 0x001279f2,   // 1210866
    AsBakePointcloudNodeTypeId      = 0x001279f3,   // 1210867
    AsReadPointcloudNodeTypeId      = 0x001279f4,   // 1210868
    AsMDLMaterialNodeTypeId         = 0x001279f5,   // 1210869
    AsMatcapNodeTypeId              = 0x001279f6,   // 1210870
    AsAngleBetweenNodeTypeId        = 0x001279f7,   // 1210871
    AsAttributesNodeTypeId          = 0x001279f8,   // 1210872
    AsMetalNodeTypeId               = 0x001279f9,   // 1210873
    AsTextureNodeTypeId             = 0x001279fa,   // 1210874
    AsCubeTextureNodeTypeId         = 0x001279fb,   // 1210875
    AsTextureInfoNodeTypeId         = 0x001279fc,   // 1210876
    AsCompositeColorNodeTypeId      = 0x001279fd,   // 1210877
    AsSwitchSurfaceNodeTypeId       = 0x001279fe,   // 1210878
    AsTexture3DNodeTypeId           = 0x001279ff,   // 1210879
    AsBumpNodeTypeId                = 0x00127a00,   // 1210880
    AsToonNodeTypeId                = 0x00127a01,   // 1210881
    AsXToonNodeTypeId               = 0x00127a02,   // 1210882
    AsFabricNodeTypeId              = 0x00127a03,   // 1210883
    AsSbsPbrMaterialNodeTypeId      = 0x00127a04,   // 1210884
    AsManifold2DNodeTypeId          = 0x00127a05,   // 1210885
    AsManifold3DNodeTypeId          = 0x00127a06,   // 1210886
    AsAscCdlNodeTypeId              = 0x00127a07,   // 1210887
    AsInvertColorNodeTypeId         = 0x00127a08,   // 1210888
    AsBlendNormalsNodeTypeId        = 0x00127a09,   // 1210889
    AsSurfaceLuminanceNodeTypeId    = 0x00127a0a,   // 1210890
    AsRaySwitchSurfaceNodeTypeId    = 0x00127a0b,   // 1210891

    LastTypeId                      = 0x00127a3f    // 1210943
};

#endif  // !APPLESEED_MAYA_TYPEIDS_H
