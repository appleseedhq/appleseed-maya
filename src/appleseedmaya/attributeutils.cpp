
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

// Interface header.
#include "attributeutils.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MFnMatrixData.h>
#include "appleseedmaya/_endmayaheaders.h"

namespace
{
    template <typename T>
    MStatus get3(const MPlug& plug, T& x, T& y, T& z)
    {
        if (!plug.isCompound())
            return MS::kFailure;

        if (plug.numChildren() != 3)
            return MS::kFailure;

        MStatus status;
        if (status) status = plug.child(0).getValue(x);
        if (status) status = plug.child(1).getValue(y);
        if (status) status = plug.child(2).getValue(z);
        return status;
    }
}

namespace AttributeUtils
{

MStatus get(const MPlug& plug, MAngle& value)
{
    return plug.getValue(value);
}


MStatus get(const MPlug& plug, MColor& value)
{
    value.a = 1.0f;
    return get3(plug, value.r, value.g, value.b);
}

MStatus get(const MPlug& plug, MPoint& value)
{
    return get3(plug, value.x, value.y, value.z);
}

MStatus get(const MPlug& plug, MVector& value)
{
    return get3(plug, value.x, value.y, value.z);
}

MStatus get(const MPlug& plug, MMatrix& value)
{
    value.setToIdentity();

    MObject matrixObject;
    MStatus status = plug.getValue(matrixObject);

    if (!status)
        return status;

    MFnMatrixData matrixDataFn(matrixObject);
    value = matrixDataFn.matrix(&status);
    return status;
}

MStatus getPlugConnectedTo(const MPlug& dstPlug, MPlug& srcPlug)
{
    if (!dstPlug.isConnected())
        return MS::kFailure;

    MStatus status;
    MPlugArray inputConnections;
    dstPlug.connectedTo(inputConnections, true, false, &status);

    if (status)
    {
        if (inputConnections.length() == 0)
            return MS::kFailure;

        srcPlug = inputConnections[0];
    }

    return status;
}

bool hasConnections(const MPlug& plug, bool input)
{
    MStatus status;

    if (!plug.isConnected(&status))
        return false;

    MPlugArray connections;
    plug.connectedTo(
        connections,
        input ? true : false,
        input ? false : true,
        &status);

    if (status)
        return connections.length() != 0;

    return false;
}

bool anyChildPlugConnected(const MPlug& plug, bool input)
{
    MStatus status;

    if (!plug.isCompound(&status))
        return false;

    if (!status)
        return false;

    int numChildren = plug.numChildren(&status);

    if (!status)
        return false;

    for (int i = 0, e = plug.numChildren(); i < e; ++i)
    {
        MPlug c = plug.child(i, &status);

        if (!status)
            continue;

        if (hasConnections(c, input))
            return true;
    }

    return false;
}

MStatus makeInput(MFnAttribute& attr, const bool keyable)
{
    attr.setStorable(true);
    attr.setReadable(true); // if it's false, it won't appear on the attr spreadsheet
    attr.setWritable(true);
    attr.setKeyable(keyable);
    return MS::kSuccess;
}

MStatus makeOutput(MFnAttribute& attr)
{
    attr.setStorable(false);
    attr.setReadable(true);
    attr.setWritable(false);
    attr.setKeyable(false);
    return MS::kSuccess;
}

}
