
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

// appleseed.renderer headers.
#include "renderer/api/shadergroup.h"

// appleseed.foundation headers.
#include "foundation/utility/containers/dictionary.h"

// Maya headers.
#define MNoVersionString
#include <maya/MFnDependencyNode.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MPxNode.h>

// Boost headers.
//#include "boost/filesystem.hpp"

// Standard library headers.
#include <cstdlib>
#include <map>
#include <string>


namespace asr = renderer;
namespace asf = foundation;

namespace
{

struct OSLShaderInfo
{
    explicit OSLShaderInfo(const asr::ShaderQuery& q)
    {
        shaderName = q.get_shader_name();
        shaderType = q.get_shader_type();
        metadata = q.get_metadata();

        paramInfo.reserve(q.get_num_params());
        for (size_t i = 0, e = q.get_num_params(); i < e; ++i)
            paramInfo.push_back(q.get_param_info(i));
    }

    std::string shaderName;
    std::string shaderType;
    asf::Dictionary metadata;
    std::vector<asf::Dictionary> paramInfo;
};

typedef std::map<unsigned int, OSLShaderInfo> OSLShaderInfoMap;
std::map<unsigned int, OSLShaderInfo> gShadersInfo;


class AppleseedShadingNode
  : public MPxNode
{
  public:
    static void* creator()
    {
        return new AppleseedShadingNode();
    }

    static MStatus initialize()
    {
        return MStatus::kSuccess;
    }

    virtual void postConstructor()
    {
        MTypeId tid = typeId();
        OSLShaderInfoMap::const_iterator it(gShadersInfo.find(tid.id()));
        assert(it != gShadersInfo.end());

        MObject thisNode = thisMObject();
        MFnDependencyNode depNode(thisNode);

        /*
        MFnNumericAttribute nAttr;
        // more FnAttribute types here...

        for (size_t i = 0, e = it->second.paramInfo.size(); i < e; ++i)
        {
            const asf::Dictionary& pinfo = it->second.paramInfo[i];

            const std::string name = pinfo.get("name");
            const bool isClosure = pinfo.get<bool>("isclosure");

            MObject attr;

            if (isClosure)
            {
                // ...
                continue;
            }

            const std::string type = pinfo.get("type");
            const bool validDefault = pinfo.get<bool>("validdefault");
            const bool isOutput = pinfo.get<bool>("isoutput");

            if (type == "float")
            {
                attr = nAttr.create(name.c_str(), name.c_str(), MFnNumericData::kFloat, 0.0);

                if (validDefault)
                {
                    const double defaultValue = pinfo.get<double>("default");
                    nAttr.setDefault(defaultValue);
                }

                if (isOutput)
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

            if (!attr.isNull())
                depNode.addAttribute(attr);
        }
        */
    }
};

} // unnamed

MStatus ShadingNodeRegistry::registerShadingNodes(MObject plugin)
{
    MFnPlugin pluginFn(plugin);

    MGlobal::displayInfo("appleseed: Registering shading nodes...");

    /*
        query = asr::ShaderQueryFactory::create(...);

        iterate shaders directories
        {
            if (isfile(filename) and ends_with(".oso")
            {
                q.open(filename);

                if (has_mayatypeid_metadata)
                {
                    typeid = metadata.get("maya_type_id")
                    classification = metadata.get("...")

                    ShaderInfo info(query);
                    gShadersInfo[typeid] = ShaderInfo;

                    pluginFn.registerNode(
                        query.get_name(),
                        MTypeid(typeid),
                        &AppleseedShaderNode::creator,
                        &AppleseedShaderNode::initialize,
                        MPxNode::kDependNode,
                        maya_classification);
                }
            }
        }
    */

    return MStatus::kSuccess;
}

MStatus ShadingNodeRegistry::unregisterShadingNodes(MObject plugin)
{
    MFnPlugin pluginFn(plugin);

    MGlobal::displayInfo("appleseed: Unregistering shading nodes...");

    for (OSLShaderInfoMap::const_iterator it = gShadersInfo.begin(), e = gShadersInfo.end(); it != e; ++it)
        pluginFn.deregisterNode(MTypeId(it->first));

    return MStatus::kSuccess;
}
