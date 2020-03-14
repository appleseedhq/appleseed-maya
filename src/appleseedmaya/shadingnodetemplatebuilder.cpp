
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016-2019 Esteban Tovagliari, The appleseedhq Organization
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
#include "shadingnodetemplatebuilder.h"

// appleseed-maya headers.
#include "appleseedmaya/shadingnodemetadata.h"
#include "appleseedmaya/shadingnoderegistry.h"
#include "appleseedmaya/utils.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.foundation headers.
#include "foundation/core/concepts/noncopyable.h"
#include "foundation/string/string.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MGlobal.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include "appleseedmaya/_endmayaheaders.h"

// Standard headers.
#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

namespace asr = renderer;
namespace asf = foundation;

namespace
{

class LayoutTree
{
  public:
    explicit LayoutTree(const OSLShaderInfo& shaderInfo)
    {
        for (size_t i = 0, e = shaderInfo.paramInfo.size(); i < e; ++i)
        {
            const OSLParamInfo& p = shaderInfo.paramInfo[i];
            if (p.page.length() != 0)
                addAttribute(p.page.asChar(), p.mayaAttributeName.asChar(), &p);
        }
    }

    LayoutTree(const std::string& name, const OSLParamInfo* paramInfo = nullptr)
      : m_name(name)
      , m_paramInfo(paramInfo)
    {
    }

    ~LayoutTree()
    {
        for (auto i : m_childrenNodes)
            delete i;
    }

    void generateAETemplate(const OSLShaderInfo& shaderInfo, MString& aeTemplate) const
    {
        std::stringstream ss;

        // Import the bump control.
        ss << "source AElambertCommon;\n";
        ss << "source AEappleseedBumpControl;\n";

        // Create the AE procedure.
        ss << "global proc AE" << shaderInfo.mayaName << "Template(string $nodeName)\n";
        ss << "{\n";
        ss << "    AEswatchDisplay $nodeName;\n";
        ss << "    editorTemplate -beginScrollLayout;\n";

        // Create layouts and add controls for each page.
        for (const auto node : m_childrenNodes)
            node->emitAETemplateFragment(ss, 1);

        // Supress hidden attributes.
        for (size_t i = 0, e = shaderInfo.paramInfo.size(); i < e; ++i)
        {
            const OSLParamInfo& p = shaderInfo.paramInfo[i];
            if (p.widget == "null")
                ss << "    editorTemplate -suppress \"" << p.mayaAttributeName << "\";\n";
        }

        // Add help
        ss << "addAttributeEditorNodeHelp(\"" << shaderInfo.mayaName
           << "\", " << "\"showHelp -absolute \\\"" << shaderInfo.shaderHelpURL
           << "\\\"\");\n;";

        // Include/call base class/node attributes.
        ss << "    AEdependNodeTemplate $nodeName;\n";

        // Show controls outside of any layout.
        ss << "    editorTemplate -addExtraControls;\n";

        // Finish procedure.
        ss << "    editorTemplate -endScrollLayout;\n";
        ss << "}\n";

        aeTemplate = ss.str().c_str();
    }

    std::vector<LayoutTree*>    m_childrenNodes;
    std::string                 m_name;
    const OSLParamInfo*         m_paramInfo;

private:
    void addAttribute(const char* page, const char* name, const OSLParamInfo* paramInfo)
    {
        std::vector<std::string> tokens;
        asf::tokenize(page, ".", tokens);
        doAddAttribute(tokens.begin(), tokens.end(), name, paramInfo);
    }

    template <typename It>
    void doAddAttribute(It f, It l, const char* name, const OSLParamInfo* paramInfo)
    {
        if (f == l)
        {
            // Add a new attribute.
            m_childrenNodes.push_back(new LayoutTree(name, paramInfo));
            return;
        }

        LayoutTree* childNode = nullptr;
        for (const auto node : m_childrenNodes)
        {
            if (*f == node->m_name)
            {
                childNode = node;
                break;
            }
        }

        if (childNode == nullptr)
        {
            // Create a new page.
            m_childrenNodes.push_back(new LayoutTree(*f));
            childNode = m_childrenNodes.back();
        }

        // Recurse.
        childNode->doAddAttribute(f + 1, l, name, paramInfo);
    }

    void emitAETemplateFragment(std::stringstream& ss, const size_t level)
    {
        if (isLayout())
        {
            // Begin layout.
            indent(ss, level);
            ss << "editorTemplate -beginLayout \"" << m_name << "\"  -collapse 0;\n";
        }
        else
        {
            // Add attribute control if not hidden.
            if (m_paramInfo->widget != "null")
            {
                indent(ss, level);

                if (m_paramInfo->widget == "maya_bump" || m_paramInfo->mayaAttributeName == "normalCamera")
                {
                    ss << "editorTemplate -callCustom \"AEappleseedShaderBumpNew\" \"AEappleseedShaderBumpReplace\" \""
                       << m_paramInfo->mayaAttributeName << "\";\n";
                }
                else if (m_paramInfo->asWidget == "ramp")
                    ss << "AEaddRampControl($nodeName + \"." << m_paramInfo->mayaAttributeName << "\");\n";
                else if (m_paramInfo->asWidget == "ramp_positions" || m_paramInfo->asWidget == "ramp_basis")
                {
                    // Ramp positions and basis params are included in the ramp control above.
                }
                else
                {
                    ss << "editorTemplate -addControl ";

                    if (m_paramInfo->help.length() != 0)
                        ss << "-ann \"" << m_paramInfo->help << "\"";

                    ss << "\"" << m_paramInfo->mayaAttributeName << "\";\n";
                }

                if (m_paramInfo->divider)
                {
                    indent(ss, level);
                    ss << "editorTemplate -addSeparator;\n";
                }
            }
        }

        // Emit children layouts and attributes.
        for (const auto node : m_childrenNodes)
            node->emitAETemplateFragment(ss, level + 1);

        if (isLayout())
        {
            // End layout.
            indent(ss, level);
            ss << "editorTemplate -endLayout;\n";
        }
    }

    bool isLayout() const
    {
        return m_paramInfo == nullptr;
    }

    void indent(std::stringstream& ss, const size_t level) const
    {
        for (size_t i = 0; i < level; ++i)
            ss << "    ";
    }
};

}

MStatus buildAndRegisterAETemplate(const OSLShaderInfo& shaderInfo)
{
    // Build a tree of layouts and attributes.
    LayoutTree attributesTree(shaderInfo);

    // Generate AETemplate procedure.
    MString aeTemplate;
    attributesTree.generateAETemplate(shaderInfo, aeTemplate);

    #if 0
        // Log AETemplate procedure.
        std::cout << "================================" << std::endl;
        std::cout << "Shader: " << shaderInfo.mayaName.asChar() << std::endl << std::endl;
        std::cout << aeTemplate << std::endl;
        std::cout << "================================" << std::endl << std::endl;
    #endif

    // Execute AETemplate procedure.
    return MGlobal::executeCommand(aeTemplate);
}
