
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

#ifndef APPLESEED_MAYA_EXPORTERS_SHAPEEXPORTER_H
#define APPLESEED_MAYA_EXPORTERS_SHAPEEXPORTER_H

// Standard headers.
#include<string>

// Boost headers.
#include "boost/shared_ptr.hpp"

// Maya headers.
#include <maya/MObject.h>
#include <maya/MObjectArray.h>

// appleseed.renderer headers.
#include "renderer/api/utility.h"

// appleseed.maya headers.
#include "appleseedmaya/exporters/dagnodeexporter.h"
#include "appleseedmaya/murmurhash.h"

// Forward declarations.
namespace renderer { class Project; }


class ShapeExporter
  : public DagNodeExporter
{
  public:

    virtual bool supportsInstancing() const;

    virtual void exportTransformMotionStep(float time);

    virtual void flushEntity() = 0;

  protected:

    ShapeExporter(const MDagPath& path, renderer::Project& project);

    renderer::TransformSequence m_transformSequence;
    MurmurHash                  m_shapeHash;
};

typedef boost::shared_ptr<ShapeExporter> ShapeExporterPtr;

#endif  // !APPLESEED_MAYA_EXPORTERS_SHAPEEXPORTER_H
