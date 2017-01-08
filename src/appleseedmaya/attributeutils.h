
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

#ifndef APPLESEED_MAYA_ATTRIBUTE_UTILS_H
#define APPLESEED_MAYA_ATTRIBUTE_UTILS_H

// Maya headers.
#include <maya/MAngle.h>
#include <maya/MColor.h>
#include <maya/MFnAttribute.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MMatrix.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MPoint.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MVector.h>

namespace AttributeUtils
{

template<class T>
MStatus get(const MPlug& plug, T& value)
{
    return plug.getValue(value);
}

MStatus get(const MPlug& plug, MAngle& value);
MStatus get(const MPlug& plug, MColor& value);
MStatus get(const MPlug& plug, MPoint& value);
MStatus get(const MPlug& plug, MVector& value);
MStatus get(const MPlug& plug, MMatrix& value);

template<class T>
MStatus get(const MFnDependencyNode& depNodeFn, const MString& attrName, T& value)
{
    MStatus status;
    MPlug plug = depNodeFn.findPlug(attrName, &status);
    if (!status)
        return status;

    return get(plug, value);
}

template<class T>
MStatus get(const MObject& node, const MString& attrName, T& value)
{
    MFnDependencyNode depNodeFn(node);
    return get(depNodeFn, attrName, value);
}

MStatus getPlugConnectedTo(const MPlug& dstPlug, MPlug& srcPlug);

bool hasConnections(const MPlug& plug, bool input);

bool anyChildPlugConnected(const MPlug& plug, bool input);

MStatus makeInput(MFnAttribute& attr);
MStatus makeOutput(MFnAttribute& attr);

} // AttributeUtils.

#endif  // !APPLESEED_MAYA_ATTRIBUTE_UTILS_H
