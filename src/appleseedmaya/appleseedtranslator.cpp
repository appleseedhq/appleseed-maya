
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
#include "appleseedmaya/appleseedtranslator.h"

// Standard headers.
#include <stdlib.h>
#include <string>
#include <vector>

// Maya headers.
#include <maya/MCommonRenderSettingsData.h>
#include <maya/MRenderUtil.h>

// appleseed.foundation headers.
#include "foundation/utility/string.h"

// appleseed.maya headers.
#include "appleseedmaya/appleseedsession.h"
#include "appleseedmaya/logger.h"

namespace asf = foundation;

const MString AppleseedTranslator::translatorName("appleseed");

AppleseedTranslator::AppleseedTranslator()
{
}

void* AppleseedTranslator::creator()
{
    return new AppleseedTranslator();
}

MString AppleseedTranslator::defaultExtension() const
{
    return "appleseed";
}

MStatus AppleseedTranslator::writer(
    const MFileObject&                  file,
    const MString&                      opts,
    MPxFileTranslator::FileAccessMode   mode)
{
    RENDERER_LOG_DEBUG("AppleseedTranslator::write called, options = %s", opts.asChar());

    AppleseedSession::Options options;

    // Parse the options string.
    std::vector<std::string> tokens;
    asf::tokenize(
        asf::trim_both(opts.asChar()),
        ";",
        tokens);

    std::vector<std::string> optNameValue;
    for(size_t i = 0, e = tokens.size(); i < e; ++i)
    {
        optNameValue.clear();
        asf::tokenize(
            asf::trim_both(tokens[i]),
            "=",
            optNameValue);

        if(optNameValue.size() == 2)
        {
            if(optNameValue[0] == "activeCamera")
                options.m_camera = optNameValue[1].c_str();
            else if(optNameValue[0] == "exportAnim")
                options.m_sequence = (optNameValue[1] == "true");
            else if(optNameValue[0] == "startFrame")
                options.m_firstFrame = atoi(optNameValue[1].c_str());
            else if(optNameValue[0] == "endFrame")
                options.m_lastFrame = atoi(optNameValue[1].c_str());
            else if(optNameValue[0] == "stepFrame")
                options.m_frameStep = atoi(optNameValue[1].c_str());
            else
            {
                RENDERER_LOG_WARNING(
                    "Unknown option %s in appleseed translator.", tokens[i].c_str());
            }
        }
        else
        {
            RENDERER_LOG_WARNING(
                "Bad option %s in appleseed translator.", tokens[i].c_str());
        }
    }

    if(MPxFileTranslator::kExportAccessMode == mode)
        options.m_selectionOnly = false;
    else if(MPxFileTranslator::kExportActiveAccessMode == mode)
        options.m_selectionOnly = true;

    // Get width and height from render globals.
    MCommonRenderSettingsData renderSettings;
    MRenderUtil::getCommonRenderSettings(renderSettings);
    options.m_width = renderSettings.width;
    options.m_height = renderSettings.height;

    // Export the scene.
    return AppleseedSession::projectExport(file.fullName(), options);
}

bool AppleseedTranslator::haveWriteMethod() const
{
    return true;
}
