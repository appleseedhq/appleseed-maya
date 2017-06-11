
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2017 Esteban Tovagliari, The appleseedhq Organization
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

#ifndef APPLESEED_MAYA_ALPHA_MAP_NODE_H
#define APPLESEED_MAYA_ALPHA_MAP_NODE_H

// Maya headers.
#include <maya/MObject.h>
#include <maya/MPxNode.h>
#include <maya/MTypeId.h>
#include "appleseedmaya/mayaheaderscleanup.h"


//
// AlphaMapNode
//
//  To support correctly Maya scene archiving we use a custom node to store
//  alpha map related attributes instead of using extension attributes in
//  shape nodes. This way we can archive alpha maps when the scene is archived.
//

class AlphaMapNode
  : public MPxNode
{
  public:
    static const MString nodeName;
    static const MTypeId id;

    static void* creator();
    static MStatus initialize();

    MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;

    MStringArray getFilesToArchive(
        bool    shortName,
        bool    unresolvedName,
        bool    markCouldBeImageSequence) const override;

    void getExternalContent(MExternalContentInfoTable& table) const override;
    void setExternalContent(const MExternalContentLocationTable& table) override;

  private:
    static MObject m_message;
    static MObject m_map;
};

#endif  // !APPLESEED_MAYA_ALPHA_MAP_NODE_H
