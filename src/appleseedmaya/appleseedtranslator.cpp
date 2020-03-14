
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
#include "appleseedtranslator.h"

// appleseed-maya headers.
#include "appleseedmaya/appleseedsession.h"
#include "appleseedmaya/logger.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.foundation headers.
#include "foundation/string/string.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MCommonRenderSettingsData.h>
#include <maya/MRenderUtil.h>
#include "appleseedmaya/_endmayaheaders.h"

// Boost headers.
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/convenience.hpp"
#include "boost/filesystem/operations.hpp"

// Standard headers.
#include <cstdlib>
#include <string>
#include <vector>

namespace bfs = boost::filesystem;
namespace asf = foundation;

namespace
{

void parseOptions(
    const MString&                      optsString,
    MPxFileTranslator::FileAccessMode   mode,
    AppleseedSession::Options&          options)
{
    // Break the options string into a list of individual options.
    std::vector<std::string> tokens;
    asf::tokenize(
        asf::trim_both(optsString.asChar()),
        ";",
        tokens);

    // Parse each option=value pair.
    std::vector<std::string> optNameValue;
    for (size_t i = 0, e = tokens.size(); i < e; ++i)
    {
        optNameValue.clear();
        asf::tokenize(
            asf::trim_both(tokens[i]),
            "=",
            optNameValue);

        if (optNameValue.size() == 2)
        {
            if (optNameValue[0] == "activeCamera")
                options.m_camera = optNameValue[1].c_str();
            else if (optNameValue[0] == "exportAnim")
                options.m_sequence = (optNameValue[1] == "true");
            else if (optNameValue[0] == "startFrame")
                options.m_firstFrame = atoi(optNameValue[1].c_str());
            else if (optNameValue[0] == "endFrame")
                options.m_lastFrame = atoi(optNameValue[1].c_str());
            else if (optNameValue[0] == "stepFrame")
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

    if (MPxFileTranslator::kExportAccessMode == mode)
        options.m_selectionOnly = false;
    else if (MPxFileTranslator::kExportActiveAccessMode == mode)
        options.m_selectionOnly = true;

    // Get width and height from render globals.
    MCommonRenderSettingsData renderSettings;
    MRenderUtil::getCommonRenderSettings(renderSettings);
    options.m_width = renderSettings.width;
    options.m_height = renderSettings.height;
}

}

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
    parseOptions(opts, mode, options);

    return AppleseedSession::projectExport(file.fullName(), options);
}

bool AppleseedTranslator::haveWriteMethod() const
{
    return true;
}

const MString AppleseedzTranslator::translatorName("appleseedz");

AppleseedzTranslator::AppleseedzTranslator()
{
}

void* AppleseedzTranslator::creator()
{
    return new AppleseedzTranslator();
}

MString AppleseedzTranslator::defaultExtension() const
{
    return "appleseedz";
}

MStatus AppleseedzTranslator::writer(
    const MFileObject&                  file,
    const MString&                      opts,
    MPxFileTranslator::FileAccessMode   mode)
{
    RENDERER_LOG_DEBUG("AppleseedzTranslator::write called, options = %s", opts.asChar());

    AppleseedSession::Options options;
    parseOptions(opts, mode, options);

    // Write the bounding box of the project scene.
    //options.m_writeBoundingBox = false;

    // Export the project in a tmp directory.
    const bfs::path originalProjectPath(file.fullName().asChar());
    const bfs::path packDirectory(originalProjectPath.parent_path() / "_asTmpProjectPacking");

    if (!bfs::create_directory(packDirectory))
    {
        RENDERER_LOG_ERROR(
            "Could not create tmp packing directory: %s",
            packDirectory.string().c_str());
        return MS::kFailure;
    }

    // Pack the tmp project.
    const bfs::path packedProjectPath(packDirectory / originalProjectPath.filename());
    const MStatus result = AppleseedSession::projectExport(packedProjectPath.string().c_str(), options);

    // Move the packed project to the requested place.
    if (result)
        bfs::rename(packedProjectPath, originalProjectPath);

    // Cleanup packing directory.
    bfs::remove_all(packDirectory);

    return result;
}

bool AppleseedzTranslator::haveWriteMethod() const
{
    return true;
}
