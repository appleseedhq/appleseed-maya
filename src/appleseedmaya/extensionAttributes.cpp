
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
#include "appleseedmaya/extensionAttributes.h"

// Maya headers.
#include <maya/MDGModifier.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnStringData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MNodeClass.h>

// appleseed.maya headers.
#include "appleseedmaya/logger.h"

namespace
{

MStatus makeInput(MFnAttribute& attr)
{
    attr.setStorable(true);
    attr.setWritable(true);
    attr.setReadable(true);
    attr.setKeyable(true);
    return MS::kSuccess;
}

MStatus makeOutput(MFnAttribute& attr)
{
    attr.setStorable(false);
    attr.setReadable(true);
    attr.setWritable(false);
    attr.setKeyable(false);
    //attr.setHidden(true);
    return MS::kSuccess;
}

void addShadingEngineExtensionAttrs()
{
    MStatus status;

    MFnTypedAttribute typedAttrFn;
    MObject attr = typedAttrFn.create(
        "asSurfaceShader",
        "asSurfaceShader",
        MFnData::kAny);
    typedAttrFn.setNiceNameOverride("asSurfaceShader");
    typedAttrFn.setStorable(true);
    typedAttrFn.setWritable(true);
    typedAttrFn.setReadable(true);

    MNodeClass nodeClass("shadingEngine");
    MDGModifier modifier;

    modifier.addExtensionAttribute(nodeClass, attr);
    modifier.doIt();
}

} // unnamed.

MStatus addExtensionAttributes()
{
    addShadingEngineExtensionAttrs();

    return MS::kSuccess;
}
