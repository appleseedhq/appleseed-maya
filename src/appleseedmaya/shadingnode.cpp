
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
#include <maya/MFnNumericAttribute.h>

// appleseed.maya headers.
#include <appleseedmaya/shadingnoderegistry.h>

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

void ShadingNode::postConstructor()
{
    setMPSafe(true);

    m_shaderInfo = ShadingNodeRegistry::getShaderInfo(typeName());
    assert(m_shaderInfo != 0);

    MObject thisNode = thisMObject();
    MFnDependencyNode depNodeFn(thisNode);

    // Create the output attributes.
    MFnNumericAttribute numAttrFn;

    MObject outColor = numAttrFn.createColor("outColor", "oc");
    numAttrFn.setKeyable(false);
    numAttrFn.setStorable(false);
    numAttrFn.setReadable(true);
    numAttrFn.setWritable(false);
    depNodeFn.addAttribute(outColor, MFnDependencyNode::kLocalDynamicAttr);

    for(size_t i = 0, e = m_shaderInfo->paramInfo.size(); i < e; ++i)
    {
        const OSLParamInfo& p = m_shaderInfo->paramInfo[i];

        MObject attr;

        if(p.paramType == "color")
        {
            //	CHECK_MSTATUS( nAttr.setUsedAsColor( true ) );
        }
        else if(p.paramType == "float")
        {
        }
        else if(p.paramType == "float[2]")
        {
        }
        else if(p.paramType == "int")
        {
        }
        else if(p.paramType == "normal")
        {
        }
        else if(p.paramType == "point")
        {
        }
        else if(p.paramType == "pointer")
        {
            // closures.
        }
        else if(p.paramType == "vector")
        {
        }
        else if(p.paramType == "string")
        {
        }

        if(attr.isNull())
            continue;

        if(p.isOutput)
        {
            /*
            CHECK_MSTATUS( nAttr.setHidden( false ) );
            CHECK_MSTATUS( nAttr.setReadable( true ) );
            CHECK_MSTATUS( nAttr.setWritable( false ) );
            */
        }
        else
        {
        }

        depNodeFn.addAttribute(outColor, MFnDependencyNode::kLocalDynamicAttr);
    }

    /*
    MFnNumericAttribute nAttr;
    // more FnAttribute types here...

    for(size_t i = 0, e = it->second.paramInfo.size(); i < e; ++i)
    {
        const asf::Dictionary& pinfo = it->second.paramInfo[i];

        const std::string name = pinfo.get("name");
        const bool isClosure = pinfo.get<bool>("isclosure");

        MObject attr;

        if(isClosure)
        {
            // ...
            continue;
        }

        const std::string type = pinfo.get("type");
        const bool validDefault = pinfo.get<bool>("validdefault");
        const bool isOutput = pinfo.get<bool>("isoutput");

        if(type == "float")
        {
            attr = nAttr.create(name.c_str(), name.c_str(), MFnNumericData::kFloat, 0.0);

            if(validDefault)
            {
                const double defaultValue = pinfo.get<double>("default");
                nAttr.setDefault(defaultValue);
            }

            if(isOutput)
            {

            }
            else
            {
                nAttr.setStorable(true);
                nAttr.setKeyable(true);
            }
        }
        // more types here...
        else
        {
            // warning: unhandled type...
        }

        if(!attr.isNull())
            depNode.addAttribute(attr);
    }
    */
}
