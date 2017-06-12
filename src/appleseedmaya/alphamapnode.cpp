
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

// Interface header.
#include "alphamapnode.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/config.h"
#include "appleseedmaya/typeids.h"

// Maya headers.
#include <maya/MFileObject.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include "appleseedmaya/_endmayaheaders.h"

const MString AlphaMapNode::nodeName("appleseedAlphaMap");
const MTypeId AlphaMapNode::id(AlphaMapNodeTypeId);

MObject AlphaMapNode::m_message;
MObject AlphaMapNode::m_map;

void* AlphaMapNode::creator()
{
    return new AlphaMapNode();
}

MStatus AlphaMapNode::initialize()
{
    MStatus status;
    MFnMessageAttribute msgAttrFn;
    MFnTypedAttribute typedAttrFn;

    // Shape connection.
    m_message = msgAttrFn.create("shape", "shape", &status);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to create alpha map attribute");
    AttributeUtils::makeOutput(msgAttrFn);
    status = addAttribute(m_message);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add alpha map attribute");

    // Map.
    m_map = typedAttrFn.create("map", "map", MFnData::kString);
    typedAttrFn.setUsedAsFilename(true);
    status = addAttribute(m_map);
    APPLESEED_MAYA_CHECK_MSTATUS_RET_MSG(status, "appleseedMaya: Failed to add alpha map attribute");

    return status;
}

MStatus AlphaMapNode::compute(const MPlug& plug, MDataBlock& dataBlock)
{
    return MS::kSuccess;
}

MStringArray AlphaMapNode::getFilesToArchive(
    bool    shortName,
    bool    unresolvedName,
    bool    markCouldBeImageSequence) const
{
    MStringArray files;
    MStatus status = MS::kSuccess;

    MPlug fileNamePlug(thisMObject(), m_map);
    MString fileName = fileNamePlug.asString(MDGContext::fsNormal, &status);

    if (status == MS::kSuccess && fileName.length() > 0)
    {
        if(unresolvedName)
            files.append(fileName);
        else
        {
            //unresolvedName is false, resolve the path via MFileObject.
            MFileObject fileObject;
            fileObject.setRawFullName(fileName);
            files.append(fileObject.resolvedFullName());
        }
    }

    return files;
}

void AlphaMapNode::getExternalContent(MExternalContentInfoTable& table) const
{
   addExternalContentForFileAttr(table, m_map);
   MPxNode::getExternalContent(table);
}

void AlphaMapNode::setExternalContent(const MExternalContentLocationTable& table)
{
   setExternalContentForFileAttr(m_map, table);
   MPxNode::setExternalContent(table);
}
