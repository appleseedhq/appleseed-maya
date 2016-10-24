
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
#define MNoVersionString
#include <maya/MFnDependencyNode.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MPxNode.h>
#include <maya/MTypeId.h>

// appleseed.foundation headers.
#include "foundation/utility/autoreleaseptr.h"

// appleseed maya headers.
#include "appleseedmaya/shadingnode.h"
#include "appleseedmaya/shadingnodetemplatebuilder.h"

namespace bfs = boost::filesystem;
namespace asr = renderer;
namespace asf = foundation;


OSLShaderInfo::OSLShaderInfo()
    : typeId(0)
{
}

OSLShaderInfo::OSLShaderInfo(const asr::ShaderQuery& q)
    : typeId(0)
{
    shaderName = q.get_shader_name();
    shaderType = q.get_shader_type();
    metadata = q.get_metadata();

    getMetadataValue("mayaName", mayaName);
    getMetadataValue("mayaClassification", mayaClassification);
    getMetadataValue<unsigned int>("mayaTypeId", typeId);

    paramInfo.reserve(q.get_num_params());
    for (size_t i = 0, e = q.get_num_params(); i < e; ++i)
        paramInfo.push_back(q.get_param_info(i));
}


namespace
{

typedef std::map<std::string, OSLShaderInfo> OSLShaderInfoMap;
std::map<std::string, OSLShaderInfo> gShadersInfo;

bool registerShader(
    const bfs::path& shaderPath,
    MFnPlugin& pluginFn,
    asr::ShaderQuery& query)
{
    if(query.open(shaderPath.string().c_str()))
    {
        OSLShaderInfo shaderInfo(query);

        if(shaderInfo.mayaName.empty())
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

        if(shaderInfo.mayaClassification.empty())
        {
            std::cout << "  Skipping shader " << shaderInfo.shaderName
                      << ". No mayaClassification found." << std::endl;
            return false;
        }

        gShadersInfo[shaderInfo.mayaName] = shaderInfo;

        if(shaderInfo.typeId != 0)
        {
            // This shader is not a builtin node or a node from other plugin.
            // Create a MPxNode for this shader.
            MString classification(shaderInfo.mayaClassification.c_str());
            MStatus status = pluginFn.registerNode(
                MString(shaderInfo.mayaName.c_str()),
                MTypeId(shaderInfo.typeId),
                &ShadingNode::creator,
                &ShadingNode::initialize,
                MPxNode::kDependNode,
                &classification);

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
        registerShadersInDirectory(shaderPaths[i], pluginFn, *query);
    }

    return MS::kSuccess;
}

MStatus ShadingNodeRegistry::unregisterShadingNodes(MObject plugin)
{
    MFnPlugin pluginFn(plugin);

    MGlobal::displayInfo("appleseed: Unregistering shading nodes...");

    for (OSLShaderInfoMap::const_iterator it = gShadersInfo.begin(), e = gShadersInfo.end(); it != e; ++it)
    {
        const OSLShaderInfo& shaderInfo(it->second);

        if(shaderInfo.typeId != 0)
            pluginFn.deregisterNode(MTypeId(shaderInfo.typeId));
    }

    return MS::kSuccess;
}

const OSLShaderInfo *ShadingNodeRegistry::getShaderInfo(const MString& nodeName)
{
    OSLShaderInfoMap::const_iterator it = gShadersInfo.find(nodeName.asChar());

    if(it == gShadersInfo.end())
        return 0;

    return &it->second;
}
