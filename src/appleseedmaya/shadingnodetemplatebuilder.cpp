
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
#include "appleseedmaya/shadingnodetemplatebuilder.h"

// Standard headers.
#include <algorithm>
#include <iostream>
#include <sstream>

// Maya headers.
#include <maya/MGlobal.h>

// appleseed.maya headers.
#include <appleseedmaya/shadingnoderegistry.h>

ShadingNodeTemplateBuilder::ShadingNodeTemplateBuilder(const OSLShaderInfo& shaderInfo)
{
    std::vector<MString> pages;
    buildPageList(shaderInfo, pages);

    std::stringstream ss;
    // Import the bump control.
    ss << "source AElambertCommon;\n";

    ss << "global proc AE" << shaderInfo.mayaName << "Template(string $nodeName)\n";
    ss << "{\n";
    ss << "    AEswatchDisplay $nodeName;\n";
    ss << "    editorTemplate -beginScrollLayout;\n";

    // Create layout and add controls for each page.
    for (size_t i = 0, ie = pages.size(); i < ie; ++i)
    {
        ss << "    editorTemplate -beginLayout \"" << pages[i] << "\"  -collapse 0;\n";

        for(size_t j = 0, je = shaderInfo.paramInfo.size(); j < je; ++j)
        {
            const OSLParamInfo& p = shaderInfo.paramInfo[j];

            if (p.widget == "null")
                continue;

            if (p.page == pages[i])
            {
                // Special case for Maya's bump. We want to reuse the bump control.
                // Maybe this should be specified by some metadata entry instead of
                // by attribute name?
                if (p.mayaAttributeName == "normalCamera")
                    ss << "        editorTemplate -callCustom \"AEshaderBumpNew\" \"AEshaderBumpReplace\" \"normalCamera\";\n";
                else
                {
                    ss << "        editorTemplate -addControl ";

                    if (p.help.length() != 0)
                        ss << "-ann \"" << p.help << "\"";

                    ss << "\"" << p.mayaAttributeName << "\"";
                    ss << ";\n";
                }

                if (p.divider)
                    ss << "        editorTemplate -addSeparator;\n";
            }
        }

        ss << "    editorTemplate -endLayout;\n";
    }

    // Supress hidden attributes.
    for(size_t i = 0, e = shaderInfo.paramInfo.size(); i < e; ++i)
    {
        const OSLParamInfo& p = shaderInfo.paramInfo[i];
        if (p.widget == "null")
            ss << "    editorTemplate -suppress \"" << p.mayaAttributeName << "\";\n";
    }

    // Include/call base class/node attributes.
    ss << "    AEdependNodeTemplate $nodeName;\n";

    ss << "    editorTemplate -addExtraControls;\n";
    ss << "    editorTemplate -endScrollLayout;\n";
    ss << "}\n";

    m_melTemplate = ss.str().c_str();
}

MStatus ShadingNodeTemplateBuilder::registerAETemplate() const
{
    return MGlobal::executeCommand(m_melTemplate);
}

void ShadingNodeTemplateBuilder::logAETemplate() const
{
    std::cout << m_melTemplate << std::endl;
}

void ShadingNodeTemplateBuilder::buildPageList(
    const OSLShaderInfo&    shaderInfo,
    std::vector<MString>&   pages) const
{
    // Naive and slow, but the number of pages and parameters should be small,
    // and we do the work only once at startup.
    for(size_t i = 0, e = shaderInfo.paramInfo.size(); i < e; ++i)
    {
        const OSLParamInfo& p = shaderInfo.paramInfo[i];
        if (p.page.length() != 0)
        {
            if (std::find(pages.begin(), pages.end(), p.page) == pages.end())
                pages.push_back(p.page);
        }
    }
}
