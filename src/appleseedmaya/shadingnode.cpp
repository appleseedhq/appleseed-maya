
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
#include "appleseedmaya/shadingnode.h"

// Maya headers.
#include <maya/MFnDependencyNode.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>

// appleseed.maya headers.
#include "appleseedmaya/logger.h"
#include "appleseedmaya/shadingnoderegistry.h"

void *ShadingNode::creator()
{
    return new ShadingNode();
}

MStatus ShadingNode::initialize()
{
    return MS::kSuccess;
}

ShadingNode::ShadingNode()
  : m_shaderInfo(0)
{
}

namespace
{

void makeInput(MFnAttribute& attr)
{
    attr.setKeyable(true);
    attr.setStorable(true);
    attr.setReadable(true);
    attr.setWritable(true);
}

void makeOutput(MFnAttribute& attr)
{
    attr.setKeyable(false);
    attr.setStorable(false);
    attr.setReadable(true);
    attr.setWritable(false);
}

MObject createPointAttribute(
    MFnNumericAttribute&    numAttrFn,
    const OSLParamInfo&     p)
{
    MObject attr = numAttrFn.createPoint(
        p.mayaAttributeName,
        p.mayaAttributeShortName);

    if (!p.isOutput && p.validDefault)
    {
        // todo: set default here...
    }

    return attr;
}

void initializeAttribute(MFnAttribute& attr, const OSLParamInfo& p)
{
    if (p.label.length() != 0)
        attr.setNiceNameOverride(p.label);

    if (p.isOutput)
        makeOutput(attr);
    else
        makeInput(attr);
}

}

void ShadingNode::postConstructor()
{
    MPxNode::postConstructor();
    setMPSafe(true);

    m_shaderInfo = ShadingNodeRegistry::getShaderInfo(typeName());
    assert(m_shaderInfo != 0);

    MObject thisNode = thisMObject();
    MFnDependencyNode depNodeFn(thisNode);

    for(size_t i = 0, e = m_shaderInfo->paramInfo.size(); i < e; ++i)
    {
        const OSLParamInfo& p = m_shaderInfo->paramInfo[i];

        MObject attr;

        if (strcmp(p.paramType.asChar(), "color") == 0)
        {
            MFnNumericAttribute numAttrFn;
            attr = numAttrFn.createColor(
                p.mayaAttributeName,
                p.mayaAttributeShortName);

            numAttrFn.setUsedAsColor(true);

            if (!p.isOutput && p.validDefault)
            {
                // todo: set default here...
            }

            initializeAttribute(numAttrFn, p);
        }
        else if (strcmp(p.paramType.asChar(), "float") == 0)
        {
            MFnNumericAttribute numAttrFn;
            attr = numAttrFn.create(
                p.mayaAttributeName,
                p.mayaAttributeShortName,
                MFnNumericData::kFloat);

            if (!p.isOutput && p.validDefault)
            {
                // todo: set default here...
            }

            initializeAttribute(numAttrFn, p);
        }
        else if (strcmp(p.paramType.asChar(), "float[2]") == 0)
        {
            MFnNumericAttribute numAttrFn;
            // We assume that float[2] are uvs.
            MObject child1 = numAttrFn.create("uCoord", "u", MFnNumericData::kFloat);
            MObject child2 = numAttrFn.create("vCoord", "v", MFnNumericData::kFloat);
            attr = numAttrFn.create(
                p.mayaAttributeName,
                p.mayaAttributeShortName,
                child1,
                child2);
        }
        else if (strcmp(p.paramType.asChar(), "int") == 0)
        {
            // Check to see if we need to create an int, bool or enum
            if (strcmp(p.widget.asChar(), "mapper") == 0)
            {
                MFnEnumAttribute enumAttrFn;
            }
            if (strcmp(p.widget.asChar(), "checkbox") == 0)
            {
                MFnNumericAttribute numAttrFn;
            }
            else
            {
                MFnNumericAttribute numAttrFn;
            }
        }
        else if (strcmp(p.paramType.asChar(), "matrix") == 0)
        {
            MFnMatrixAttribute matrixAttrFn;
            attr = matrixAttrFn.create(
                p.mayaAttributeName,
                p.mayaAttributeShortName,
                MFnMatrixAttribute::kFloat);

            initializeAttribute(matrixAttrFn, p);
        }
        else if (strcmp(p.paramType.asChar(), "normal") == 0)
        {
            MFnNumericAttribute numAttrFn;
            attr = createPointAttribute(numAttrFn, p);
            initializeAttribute(numAttrFn, p);
        }
        else if (strcmp(p.paramType.asChar(), "point") == 0)
        {
            MFnNumericAttribute numAttrFn;
            attr = createPointAttribute(numAttrFn, p);
            initializeAttribute(numAttrFn, p);
        }
        else if (strcmp(p.paramType.asChar(), "pointer") == 0) // closure color
        {
            MFnNumericAttribute numAttrFn;
            attr = numAttrFn.createColor(
                p.mayaAttributeName,
                p.mayaAttributeShortName);

            if (!p.isOutput && p.validDefault)
            {
                // todo: set default here...
            }

            initializeAttribute(numAttrFn, p);
        }
        else if (strcmp(p.paramType.asChar(), "vector") == 0)
        {
            MFnNumericAttribute numAttrFn;
            attr = createPointAttribute(numAttrFn, p);
            initializeAttribute(numAttrFn, p);
        }
        else if (strcmp(p.paramType.asChar(), "string") == 0)
        {
            // Check to see if we need to create a string or an enum
            if (strcmp(p.widget.asChar(), "popup") == 0)
            {
                MFnEnumAttribute enumAttrFn;
                // todo: create enum attribute here...
            }
            else
            {
                MFnTypedAttribute typedAttrFn;
                attr = typedAttrFn.create(
                    p.mayaAttributeName,
                    p.mayaAttributeShortName,
                    MFnData::kString);

                if (!p.isOutput && p.validDefault)
                {
                    // todo: set default here...
                }

                initializeAttribute(typedAttrFn, p);
            }
        }
        else
        {
            RENDERER_LOG_WARNING(
                "Ignoring param %s of shader %s",
                p.paramName.asChar(),
                m_shaderInfo->shaderName.asChar());
            continue;
        }

        if (!attr.isNull())
            depNodeFn.addAttribute(attr, MFnDependencyNode::kLocalDynamicAttr);
    }
}
