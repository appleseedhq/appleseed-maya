
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
#include "extensionattributes.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/logger.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MDGModifier.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnStringData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MNodeClass.h>
#include <maya/MString.h>
#include "appleseedmaya/_endmayaheaders.h"

// Standard headers.
#include <array>

namespace
{
    const MString g_extensionsCategory("appleseedMaya");

    template <typename T>
    MObject createNumericAttribute(
        MFnNumericAttribute&    numAttrFn,
        const MString&          longName,
        const MString&          shortName,
        MFnNumericData::Type    type,
        const T                 defaultValue,
        MStatus&                status)
    {
        MObject attr = numAttrFn.create(
            longName,
            shortName,
            type,
            defaultValue,
            &status);
        numAttrFn.addToCategory(g_extensionsCategory);
        return attr;
    }

    void addVisibilityExtensionAttributes(
        const MNodeClass&   nodeClass,
        MDGModifier&        modifier)
    {
        MStatus status;

        MFnNumericAttribute numAttrFn;
        MObject attr = createNumericAttribute<bool>(
            numAttrFn,
            "asVisibilityCamera",
            "asVisibilityCamera",
            MFnNumericData::kBoolean,
            true,
            status);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        attr = createNumericAttribute<bool>(
            numAttrFn,
            "asVisibilityLight",
            "asVisibilityLight",
            MFnNumericData::kBoolean,
            true,
            status);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        attr = createNumericAttribute<bool>(
            numAttrFn,
            "asVisibilityShadow",
            "asVisibilityShadow",
            MFnNumericData::kBoolean,
            true,
            status);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        attr = createNumericAttribute<bool>(
            numAttrFn,
            "asVisibilityDiffuse",
            "asVisibilityDiffuse",
            MFnNumericData::kBoolean,
            true,
            status);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        attr = createNumericAttribute<bool>(
            numAttrFn,
            "asVisibilitySpecular",
            "asVisibilitySpecular",
            MFnNumericData::kBoolean,
            true,
            status);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        attr = createNumericAttribute<bool>(
            numAttrFn,
            "asVisibilityGlossy",
            "asVisibilityGlossy",
            MFnNumericData::kBoolean,
            true,
            status);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);
    }

    void addMeshExtensionAttributes()
    {
        MStatus status;

        MNodeClass nodeClass("mesh");
        MDGModifier modifier;

        MFnMessageAttribute msgAttrFn;
        MObject attr = msgAttrFn.create("asAlphaMap", "asAlphaMap", &status);
        AttributeUtils::makeInput(msgAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        MFnNumericAttribute numAttrFn;
        attr = createNumericAttribute<int>(
            numAttrFn,
            "asMediumPriority",
            "asMediumPriority",
            MFnNumericData::kInt,
            0,
            status);
        numAttrFn.setMin(0);
        numAttrFn.setSoftMax(10);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        attr = createNumericAttribute<bool>(
            numAttrFn,
            "asIsPhotonTarget",
            "asIsPhotonTarget",
            MFnNumericData::kBoolean,
            false,
            status);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        attr = createNumericAttribute<float>(
            numAttrFn,
            "asShadowTerminatorCorrection",
            "asShadowTerminatorCorrection",
            MFnNumericData::kFloat,
            0.0f,
            status);
        numAttrFn.setSoftMin(0.0f);
        numAttrFn.setSoftMax(0.5f);
        numAttrFn.setMin(0.0f);
        numAttrFn.setMax(0.5f);
        numAttrFn.setKeyable(true);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        MFnTypedAttribute typedAttrFn;
        attr = typedAttrFn.create("asSubsurfaceSet", "asSubsurfaceSet", MFnData::kString);
        AttributeUtils::makeInput(typedAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        attr = createNumericAttribute<bool>(
            numAttrFn,
            "asExportUVs",
            "asExportUVs",
            MFnNumericData::kBoolean,
            true,
            status);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        attr = createNumericAttribute<bool>(
            numAttrFn,
            "asExportNormals",
            "asExportNormals",
            MFnNumericData::kBoolean,
            true,
            status);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        attr = createNumericAttribute<bool>(
            numAttrFn,
            "asSmoothTangents",
            "asSmoothTangents",
            MFnNumericData::kBoolean,
            false,
            status);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        addVisibilityExtensionAttributes(nodeClass, modifier);
        modifier.doIt();
    }

    void addAreaLightExtensionAttributes()
    {
        MNodeClass nodeClass("areaLight");
        MDGModifier modifier;

        MStatus status;

        MFnNumericAttribute numAttrFn;

        MObject attr = createNumericAttribute<float>(
            numAttrFn,
            "asIntensityScale",
            "asIntensityScale",
            MFnNumericData::kFloat,
            1.0f,
            status);
        numAttrFn.setSoftMin(0.0f);
        numAttrFn.setSoftMax(10.0f);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        attr = createNumericAttribute<float>(
            numAttrFn,
            "asExposure",
            "asExposure",
            MFnNumericData::kFloat,
            0.0f,
            status);
        numAttrFn.setSoftMin(-8.0);
        numAttrFn.setSoftMax(8.0f);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        attr = createNumericAttribute<bool>(
            numAttrFn,
            "asNormalize",
            "asNormalize",
            MFnNumericData::kBoolean,
            false,
            status);
        numAttrFn.setKeyable(false);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        addVisibilityExtensionAttributes(nodeClass, modifier);
        modifier.doIt();
    }

    void addDiracLightExtensionAttributes()
    {
        const std::array<MString, 3> lights = {"pointlight", "spotLight", "directionalLight"};

        for (const MString& light : lights)
        {
            MNodeClass nodeClass(light);
            MDGModifier modifier;

            MStatus status;

            MFnNumericAttribute numAttrFn;

            MObject attr = createNumericAttribute<bool>(
                numAttrFn,
                "asCastIndirectLight",
                "asCastIndirectLight",
                MFnNumericData::kBoolean,
                true,
                status);
            numAttrFn.setKeyable(false);
            AttributeUtils::makeInput(numAttrFn);
            modifier.addExtensionAttribute(nodeClass, attr);

            addVisibilityExtensionAttributes(nodeClass, modifier);
            modifier.doIt();
        }
    }

    void addBump2dExtensionAttributes()
    {
        MNodeClass nodeClass("bump2d");
        MDGModifier modifier;

        MStatus status;

        MFnEnumAttribute enumAttrFn;
        MObject attr = enumAttrFn.create(
            "asNormalMapMode",
            "asNormalMapMode",
            0);

        enumAttrFn.addField("Unsigned", 0);
        enumAttrFn.addField("Signed", 1);
        AttributeUtils::makeInput(enumAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        MFnNumericAttribute numAttrFn;
        attr = createNumericAttribute<bool>(
            numAttrFn,
            "asNormalMapFlipR",
            "asNormalMapFlipR",
            MFnNumericData::kBoolean,
            false,
            status);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        attr = createNumericAttribute<bool>(
            numAttrFn,
            "asNormalMapFlipG",
            "asNormalMapFlipG",
            MFnNumericData::kBoolean,
            false,
            status);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        attr = createNumericAttribute<bool>(
            numAttrFn,
            "asNormalMapSwapRG",
            "asNormalMapSwapRG",
            MFnNumericData::kBoolean,
            false,
            status);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        modifier.doIt();
    }

    void addShadingEngineExtensionAttrs()
    {
        MNodeClass nodeClass("shadingEngine");
        MDGModifier modifier;

        MStatus status;

        MFnNumericAttribute numAttrFn;

        MObject attr = createNumericAttribute<bool>(
            numAttrFn,
            "asDoubleSided",
            "asDoubleSided",
            MFnNumericData::kBoolean,
            true,
            status);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        attr = createNumericAttribute<int>(
            numAttrFn,
            "asShadingSamples",
            "asShadingSamples",
            MFnNumericData::kInt,
            1,
            status);
        numAttrFn.setMin(1);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        modifier.doIt();
    }

    void addCameraExtensionAttrs()
    {
        MNodeClass nodeClass("camera");
        MDGModifier modifier;

        MStatus status;

        MFnNumericAttribute numAttrFn;

        MObject attr = createNumericAttribute<bool>(
            numAttrFn,
            "asAutofocus",
            "asAutofocus",
            MFnNumericData::kBoolean,
            false,
            status);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        attr = createNumericAttribute<float>(
            numAttrFn,
            "asHorizontalTarget",
            "asHorizontalTarget",
            MFnNumericData::kFloat,
            0.5f,
            status);
        numAttrFn.setMin(0.0f);
        numAttrFn.setMax(1.0f);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        attr = createNumericAttribute<float>(
            numAttrFn,
            "asVerticalTarget",
            "asVerticalTarget",
            MFnNumericData::kFloat,
            0.5f,
            status);
        numAttrFn.setMin(0.0f);
        numAttrFn.setMax(1.0f);
        AttributeUtils::makeInput(numAttrFn);
        modifier.addExtensionAttribute(nodeClass, attr);

        modifier.doIt();
    }
}

MStatus addExtensionAttributes()
{
    addMeshExtensionAttributes();
    addAreaLightExtensionAttributes();
    addDiracLightExtensionAttributes();
    addBump2dExtensionAttributes();
    addShadingEngineExtensionAttrs();
    addCameraExtensionAttrs();
    return MS::kSuccess;
}
