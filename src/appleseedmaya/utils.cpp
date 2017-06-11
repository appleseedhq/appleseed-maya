
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

// Interface header.
#include "appleseedmaya/utils.h"

// Standard headers.
#include <algorithm>
#include <list>

// Boost headers.
#include "boost/thread/locks.hpp"
#include "boost/thread/mutex.hpp"

// Maya headers.
#include <maya/MDagPath.h>
#include <maya/MEventMessage.h>
#include <maya/MObject.h>
#include <maya/MSelectionList.h>
#include "appleseedmaya/mayaheaderscleanup.h"

// appleseed.maya headers.
#include "appleseedmaya/exceptions.h"

MStatus getDependencyNodeByName(const MString& name, MObject& node)
{
    MSelectionList selList;
    selList.add(name);

    if (selList.isEmpty())
        return MS::kFailure;

    return selList.getDependNode(0, node);
}

MStatus getDagPathByName(const MString& name, MDagPath& dag)
{
    MSelectionList selList;
    selList.add(name);

    if (selList.isEmpty())
        return MS::kFailure;

    return selList.getDagPath(0, dag);
}

boost::shared_ptr<Computation> Computation::create()
{
    return boost::shared_ptr<Computation>(new Computation());
}

Computation::Computation()
{
    m_computation.beginComputation();
}

Computation::~Computation()
{
    m_computation.endComputation();
}

bool Computation::isInterruptRequested()
{
    return m_computation.isInterruptRequested();
}

void Computation::thowIfInterruptRequested()
{
    if (m_computation.isInterruptRequested())
        throw AbortRequested();
}
