
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

// interface header.
#include "shadingnoderegistry.h"

// Standard library headers.
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

// Boost headers.
#include "boost/filesystem.hpp"

// Maya headers.

#define MNoPluginEntry
#define MNoVersionString
#include <maya/MFnPlugin.h>

#include <maya/MFnDependencyNode.h>
#include <maya/MGlobal.h>
#include <maya/MPxNode.h>
#include <maya/MTypeId.h>

// appleseed.foundation headers.
#include "foundation/utility/autoreleaseptr.h"

// appleseed.maya headers.
#include "appleseedmaya/shadingnode.h"
#include "appleseedmaya/shadingnodetemplatebuilder.h"
#include "appleseedmaya/utils.h"


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
    std::cout << "    IsClosure = " << p.isClosure << "\n";

    if(p.isArray)
        std::cout << "    ArrayLen = " << p.arrayLen << "\n";

    if(p.isStruct)
        std::cout << "    StructName = " << p.structName << "\n";

    std::cout << "    ValidDefault = " << p.validDefault << "\n";
}

void logShader(const OSLShaderInfo& s)
{
    std::cout << "------------------------\n";

    std::cout << "Name = " << s.shaderName << " type = " << s.shaderType << "\n";
    std::cout << "Maya Name = " << s.mayaName << "\n";
    std::cout << "Maya Classification = " << s.mayaClassification << "\n";

    if(s.typeId)
        std::cout << "Maya TypeId = " << s.typeId << "\n";

    std::cout << "Params:\n";
    std::cout << "-------\n\n";

    for(size_t i = 0, e = s.paramInfo.size(); i < e; ++i)
        logParam(s.paramInfo[i]);

    std::cout << "------------------------\n";
    std::cout << std::endl;
}

typedef std::map<MString, OSLShaderInfo, MStringCompareLess> OSLShaderInfoMap;
OSLShaderInfoMap gShadersInfo;

bool doRegisterShader(
    const bfs::path& shaderPath,
    MFnPlugin& pluginFn,
    asr::ShaderQuery& query)
{
    if(query.open(shaderPath.string().c_str()))
    {
        OSLShaderInfo shaderInfo(query);

        if(shaderInfo.mayaName.length() == 0)
        {
            std::cout << "  Skipping shader " << shaderInfo.shaderName
                      << ". No mayaName found." << std::endl;
            return false;
        }

        if(gShadersInfo.count(shaderInfo.mayaName) != 0)
        {
            std::cout << "  Skipping shader " << shaderInfo.shaderName
                      << ". Already registered." << std::endl;
            return false;
        }

        if(shaderInfo.mayaClassification.length() == 0)
        {
            std::cout << "  Skipping shader " << shaderInfo.shaderName
                      << ". No mayaClassification found." << std::endl;
            return false;
        }

        gShadersInfo[shaderInfo.mayaName] = shaderInfo;

        // while debugging...
        logShader(shaderInfo);

        if(shaderInfo.typeId != 0)
        {
            // This shader is not a builtin node or a node from other plugin.
            // Create a MPxNode for this shader.
            MStatus status = pluginFn.registerNode(
                shaderInfo.mayaName,
                MTypeId(shaderInfo.typeId),
                &ShadingNode::creator,
                &ShadingNode::initialize,
                MPxNode::kDependNode,
                &shaderInfo.mayaClassification);

            if(!status)
            {
                std::cout << "  Registration of shading node for shader "
                            << shaderInfo.shaderName << " failed" << std::endl;

                gShadersInfo.erase(shaderInfo.mayaName);
                return false;
            }

            // Build and register an AE template for the node.
            ShadingNodeTemplateBuilder aeBuilder(shaderInfo);

            // While debugging...
            aeBuilder.logAETemplate();

            aeBuilder.registerAETemplate();
        }

        return true;
    }

    return false;
}

bool registerShader(
    const bfs::path& shaderPath,
    MFnPlugin& pluginFn,
    asr::ShaderQuery& query)
{
    try
    {
        return doRegisterShader(shaderPath, pluginFn, query);
    }
    catch(const asf::StringException& e)
    {
        std::cout << "Exception while querying " << shaderPath
                  << ", what = " << e.string() << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cout << "Exception while querying " << shaderPath
                  << ", what = " << e.what() << std::endl;
    }
    catch(...)
    {
        std::cout << "Unknown exception while querying " << shaderPath << std::endl;
    }

    return false;
}

void registerShadersInDirectory(
    const bfs::path& shaderDir,
    MFnPlugin& pluginFn,
    asr::ShaderQuery& query)
{
    try
    {
        if(bfs::exists(shaderDir) && bfs::is_directory(shaderDir))
        {
            bfs::directory_iterator it(shaderDir), e;
            for(; it != e; ++it)
            {
                const bfs::file_status shaderStatus = it->status();
                if(shaderStatus.type() == bfs::regular_file)
                {
                    const bfs::path& shaderPath = it->path();
                    if(shaderPath.extension() == ".oso")
                    {
                        std::cout << "Found shader " << shaderPath << std::endl;

                        if(!registerShader(shaderPath, pluginFn, query))
                            std::cout << "  Register failed for shader " << shaderPath << std::endl;
                    }
                }

                // todo: handle symlinks here?
            }
        }
    }
    catch(const bfs::filesystem_error& e)
    {
        std::cout << "appleseedMaya: filesystem, path = " << shaderDir
                  << " ,error: " << e.what() << std::endl;
    }
}

} // unnamed

MStatus ShadingNodeRegistry::registerShadingNodes(MObject plugin)
{
    MFnPlugin pluginFn(plugin);

    // Build list of dirs to look for shaders
    std::vector<bfs::path> shaderPaths;

    // Look relative to plugin
    bfs::path p(pluginFn.loadPath().asChar());
    p = p.parent_path() / "shaders";
    shaderPaths.push_back(p);

    // env vars APPLESEED_SEARCHPATH?

    asf::auto_release_ptr<asr::ShaderQuery> query =
        asr::ShaderQueryFactory::create();

    // Iterate in reverse order to allow overriding of shaders.
    for(int i = shaderPaths.size() - 1; i >= 0; --i)
    {
        std::cout << "Looking for shaders in " << shaderPaths[i] << std::endl;
        //registerShadersInDirectory(shaderPaths[i], pluginFn, *query);
    }

    return MS::kSuccess;
}

MStatus ShadingNodeRegistry::unregisterShadingNodes(MObject plugin)
{
    MFnPlugin pluginFn(plugin);

    MGlobal::displayInfo("appleseed: Unregistering shading nodes...");

    for(OSLShaderInfoMap::const_iterator it = gShadersInfo.begin(), e = gShadersInfo.end(); it != e; ++it)
    {
        const OSLShaderInfo& shaderInfo(it->second);

        if(shaderInfo.typeId != 0)
            pluginFn.deregisterNode(MTypeId(shaderInfo.typeId));
    }

    return MS::kSuccess;
}

void ShadingNodeRegistry::getShaderNodeNames(MStringArray& nodeNames)
{
    for(OSLShaderInfoMap::const_iterator it = gShadersInfo.begin(), e = gShadersInfo.end(); it != e; ++it)
        nodeNames.append(it->first);
}

const OSLShaderInfo *ShadingNodeRegistry::getShaderInfo(const MString& nodeName)
{
    OSLShaderInfoMap::const_iterator it = gShadersInfo.find(nodeName);

    if(it == gShadersInfo.end())
        return 0;

    return &it->second;
}
