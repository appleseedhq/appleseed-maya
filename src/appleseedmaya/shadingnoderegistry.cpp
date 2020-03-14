
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
#include "shadingnoderegistry.h"

// appleseed-maya headers.
#include "appleseedmaya/logger.h"
#include "appleseedmaya/shadingnode.h"
#include "appleseedmaya/shadingnodemetadata.h"
#include "appleseedmaya/shadingnodetemplatebuilder.h"
#include "appleseedmaya/utils.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/shadergroup.h"

// appleseed.foundation headers.
#include "foundation/containers/dictionary.h"
#include "foundation/memory/autoreleaseptr.h"
#include "foundation/string/string.h"
#include "foundation/utility/searchpaths.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#define MNoPluginEntry
#define MNoVersionString
#include <maya/MFnPlugin.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MStringArray.h>
#include <maya/MTypeId.h>
#include "appleseedmaya/_endmayaheaders.h"

// Boost headers.
#include "boost/filesystem.hpp"

// Standard headers.
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

namespace bfs = boost::filesystem;
namespace asr = renderer;
namespace asf = foundation;

namespace
{
    void logParam(const OSLParamInfo& p)
    {
        std::cout << "  Name = " << p.paramName << " type = " << p.paramType << "\n";
        std::cout << "    Maya Attribute = " << p.mayaAttributeName << "\n";
        std::cout << "    IsOutput = " << p.isOutput << "\n";
        std::cout << "    Widget = " << p.widget << "\n";
        std::cout << "    IsClosure = " << p.isClosure << "\n";

        if (p.isArray)
            std::cout << "    ArrayLen = " << p.arrayLen << "\n";

        if (p.isStruct)
            std::cout << "    StructName = " << p.structName << "\n";

        std::cout << "    ValidDefault = " << p.validDefault << "\n";
    }

    void logShader(const OSLShaderInfo& s)
    {
        std::cout << "------------------------\n";

        std::cout << "Name = " << s.shaderName << " type = " << s.shaderType << "\n";
        std::cout << "File Name = " << s.shaderFileName << "\n";
        std::cout << "Maya Name = " << s.mayaName << "\n";
        std::cout << "Maya Classification = " << s.mayaClassification << "\n";
        std::cout << "Shader help URL = " << s.shaderHelpURL << "\n";

        if (s.typeId)
            std::cout << "Maya TypeId = " << s.typeId << "\n";

        std::cout << "Params:\n";
        std::cout << "-------\n\n";

        for (size_t i = 0, e = s.paramInfo.size(); i < e; ++i)
            logParam(s.paramInfo[i]);

        std::cout << "------------------------\n";
        std::cout << std::endl;
    }

    typedef std::map<MString, OSLShaderInfo, MStringCompareLess> OSLShaderInfoMap;
    OSLShaderInfoMap gShadersInfo;

    bool doRegisterShader(
        const bfs::path&    shaderPath,
        MFnPlugin&          pluginFn,
        asr::ShaderQuery&   query)
    {
        if (query.open(shaderPath.string().c_str()))
        {
            // Get the shader filename without the .oso extension.
            const MString shaderFilename(shaderPath.filename().replace_extension().c_str());
            OSLShaderInfo shaderInfo(query, shaderFilename);

            if (shaderInfo.mayaName.length() == 0)
            {
                RENDERER_LOG_DEBUG(
                    "Skipping registration for OSL shader %s. No maya name metadata found.",
                    shaderInfo.shaderName.asChar());
                return false;
            }

            if (gShadersInfo.count(shaderInfo.mayaName) != 0)
            {
                RENDERER_LOG_DEBUG(
                    "Skipping registration for OSL shader %s. Already registered.",
                    shaderInfo.shaderName.asChar());
                return false;
            }

            if (shaderInfo.typeId != 0)
            {
                if (shaderInfo.mayaClassification.length() == 0)
                {
                    RENDERER_LOG_DEBUG(
                        "Skipping registration for OSL shader %s. No maya classification metadata found.",
                        shaderInfo.shaderName.asChar());
                    return false;
                }
            }

            RENDERER_LOG_DEBUG(
                "Registered OSL shader %s",
                shaderInfo.shaderName.asChar());

            #if 0
                logShader(shaderInfo);
            #endif

            gShadersInfo[shaderInfo.mayaName] = shaderInfo;

            if (shaderInfo.typeId != 0)
            {
                // This shader is not a builtin node or a node from other plugin.
                // Create a MPxNode for this shader.
                RENDERER_LOG_DEBUG(
                    "Registering MPxNode for OSL shader %s.",
                    shaderInfo.shaderName.asChar());

                ShadingNode::setCurrentShaderInfo(&shaderInfo);
                MStatus status = pluginFn.registerNode(
                    shaderInfo.mayaName,
                    MTypeId(shaderInfo.typeId),
                    &ShadingNode::creator,
                    &ShadingNode::initialize,
                    MPxNode::kDependNode,
                    &shaderInfo.mayaClassification);

                if (!status)
                {
                    RENDERER_LOG_WARNING(
                        "Registration of OSL shader %s failed, error = %s.",
                        shaderInfo.shaderName.asChar(),
                        status.errorString().asChar());

                    gShadersInfo.erase(shaderInfo.mayaName);
                    return false;
                }

                buildAndRegisterAETemplate(shaderInfo);
            }

            return true;
        }

        return false;
    }

    bool registerShader(
        const bfs::path&    shaderPath,
        MFnPlugin&          pluginFn,
        asr::ShaderQuery&   query)
    {
        try
        {
            return doRegisterShader(shaderPath, pluginFn, query);
        }
        catch (const asf::StringException& e)
        {
            RENDERER_LOG_ERROR(
                "OSL shader query for shader %s failed, error = %s.",
                shaderPath.string().c_str(),
                e.string());
        }
        catch (const std::exception& e)
        {
            RENDERER_LOG_ERROR(
                "OSL shader query for shader %s failed, error = %s.",
                shaderPath.string().c_str(),
                e.what());
        }
        catch (...)
        {
            RENDERER_LOG_ERROR(
                "OSL shader query for shader %s failed.",
                shaderPath.string().c_str());
        }

        return false;
    }

    void registerShadersInDirectory(
        const bfs::path&    shaderDir,
        MFnPlugin&          pluginFn,
        asr::ShaderQuery&   query)
    {
        try
        {
            if (bfs::exists(shaderDir) && bfs::is_directory(shaderDir))
            {
                for (bfs::directory_iterator it(shaderDir), e; it != e; ++it)
                {
                    const bfs::file_status shaderStatus = it->status();
                    if (shaderStatus.type() == bfs::regular_file)
                    {
                        const bfs::path& shaderPath = it->path();
                        if (shaderPath.extension() == ".oso")
                        {
                            RENDERER_LOG_DEBUG(
                                "Found OSL shader %s.",
                                shaderPath.string().c_str());

                            registerShader(shaderPath, pluginFn, query);
                        }
                    }

                    // TODO: should we handle symlinks here?
                }
            }
        }
        catch (const bfs::filesystem_error& e)
        {
            RENDERER_LOG_ERROR(
                "Filesystem error, path = %s, error = %s.",
                shaderDir.string().c_str(),
                e.what());
        }
    }
}

namespace ShadingNodeRegistry
{

MStatus registerShadingNodes(MObject plugin)
{
    MFnPlugin pluginFn(plugin);

    // Build list of dirs to look for shaders
    std::vector<bfs::path> shaderPaths;

    // Look relative to the plugin DLL / so file.
    bfs::path p(pluginFn.loadPath().asChar());
    p = p.parent_path() / ".." / "shaders";
    shaderPaths.push_back(p);

    // Paths from the environment.
    // TODO: maybe APPLESEED_SEARCHPATH is not the best var...?
    if (const char* envSearchPath = getenv("APPLESEED_SEARCHPATH"))
    {
        const char pathSep = asf::SearchPaths::environment_path_separator();
        std::vector<std::string> paths;
        asf::split(
            envSearchPath,
            std::string(&pathSep, 1),
            paths);

        for (size_t i = 0, e = paths.size(); i < e; ++i)
            shaderPaths.push_back(bfs::path(paths[i]));
    }

    asf::auto_release_ptr<asr::ShaderQuery> query =
        asr::ShaderQueryFactory::create();

    // Iterate in reverse order to allow overriding of shaders.
    for (int i = static_cast<int>(shaderPaths.size()) - 1; i >= 0; --i)
    {
        RENDERER_LOG_DEBUG(
            "Looking for OSL shaders in path %s.",
            shaderPaths[i].string().c_str());

        registerShadersInDirectory(shaderPaths[i], pluginFn, *query);
    }

    // Refresh the hypershade window.
    MString command("if (`window -exists createRenderNodeWindow`) {refreshCreateRenderNodeWindow(\"\");}\n");
    MGlobal::executeCommand(command);

    return MS::kSuccess;
}

MStatus unregisterShadingNodes(MObject plugin)
{
    MFnPlugin pluginFn(plugin);

    RENDERER_LOG_DEBUG("Unregistering shading nodes.");

    for (auto it = gShadersInfo.begin(), e = gShadersInfo.end(); it != e; ++it)
    {
        const OSLShaderInfo& shaderInfo(it->second);

        if (shaderInfo.typeId != 0)
            pluginFn.deregisterNode(MTypeId(shaderInfo.typeId));
    }

    // Refresh the hypershade.
    MString command("if (`window -exists createRenderNodeWindow`) {refreshCreateRenderNodeWindow(\"\");}\n");
    MGlobal::executeCommand(command);

    return MS::kSuccess;
}

void getShaderNodeNames(MStringArray& nodeNames)
{
    for (auto it = gShadersInfo.begin(), e = gShadersInfo.end(); it != e; ++it)
        nodeNames.append(it->first);
}

const OSLShaderInfo* getShaderInfo(const MString& nodeName)
{
    auto it = gShadersInfo.find(nodeName);

    if (it == gShadersInfo.end())
        return nullptr;

    return &it->second;
}

bool isShaderSupported(const MString& nodeName)
{
    return getShaderInfo(nodeName) != nullptr;
}

} // namespace ShadingNodeRegistry
