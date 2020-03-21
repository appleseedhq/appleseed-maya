
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
#include "dagnodeexporter.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/project.h"
#include "renderer/api/scene.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MAnimUtil.h>
#include <maya/MBoundingBox.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnExpression.h>
#include <maya/MGlobal.h>
#include <maya/MItDependencyGraph.h>
#include "appleseedmaya/_endmayaheaders.h"

namespace asf = foundation;
namespace asr = renderer;

DagNodeExporter::DagNodeExporter(
    const MDagPath&                 path,
    asr::Project&                   project,
    AppleseedSession::SessionMode   sessionMode)
  : m_path(path)
  , m_sessionMode(sessionMode)
  , m_project(project)
  , m_scene(*project.get_scene())
  , m_mainAssembly(*m_scene.assemblies().get_by_name("assembly"))
{
}

DagNodeExporter::~DagNodeExporter()
{
}

MObject DagNodeExporter::node() const
{
    return dagPath().node();
}

AppleseedSession::SessionMode DagNodeExporter::sessionMode() const
{
    return m_sessionMode;
}

asr::Project& DagNodeExporter::project()
{
    return m_project;
}

asr::Scene& DagNodeExporter::scene()
{
    return m_scene;
}

asr::Assembly& DagNodeExporter::mainAssembly()
{
    return m_mainAssembly;
}

void DagNodeExporter::createExporters(const AppleseedSession::IExporterFactory& exporter_factory)
{
}

bool DagNodeExporter::supportsMotionBlur() const
{
    return true;
}

void DagNodeExporter::exportCameraMotionStep(float time)
{
}

void DagNodeExporter::exportTransformMotionStep(float time)
{
}

void DagNodeExporter::exportShapeMotionStep(float time)
{
}

asf::AABB3d DagNodeExporter::boundingBox() const
{
    return asf::AABB3d();
}

asf::AABB3d DagNodeExporter::objectSpaceBoundingBox(const MDagPath& path)
{
    MFnDagNode dagNodeFn(path);
    MBoundingBox b = dagNodeFn.boundingBox();

    return asf::AABB3d(
        asf::Vector3d(b.min().x, b.min().y, b.min().z),
        asf::Vector3d(b.max().x, b.max().y, b.max().z));
}

MString DagNodeExporter::appleseedName() const
{
    return dagPath().partialPathName();
}

const MDagPath& DagNodeExporter::dagPath() const
{
    return m_path;
}

asf::Matrix4d DagNodeExporter::convert(const MMatrix& m) const
{
    asf::Matrix4d result;

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
            result(i, j) = m[j][i];
    }

    return result;
}

void DagNodeExporter::addVisibilityAttributesToParams(asr::ParamArray& params)
{
    asf::Dictionary visFlags;

    bool flag = true;
    if (AttributeUtils::get(node(), "asVisibilityCamera", flag))
        if (!flag) visFlags.insert("camera", false);

    flag = true;
    if (AttributeUtils::get(node(), "asVisibilityLight", flag))
        if (!flag) visFlags.insert("light", false);

    flag = true;
    if (AttributeUtils::get(node(), "asVisibilityShadow", flag))
        if (!flag) visFlags.insert("shadow", false);

    flag = true;
    if (AttributeUtils::get(node(), "asVisibilityDiffuse", flag))
        if (!flag) visFlags.insert("diffuse", false);

    flag = true;
    if (AttributeUtils::get(node(), "asVisibilitySpecular", flag))
        if (!flag) visFlags.insert("specular", false);

    flag = true;
    if (AttributeUtils::get(node(), "asVisibilityGlossy", flag))
        if (!flag) visFlags.insert("glossy", false);

    if (!visFlags.empty())
        params.insert("visibility", visFlags);
}

bool DagNodeExporter::isObjectRenderable(const MDagPath& path)
{
    MFnDagNode dagNodeFn(path);

    // Skip intermediate objects.
    if (dagNodeFn.isIntermediateObject())
        return false;

    // Skip templated objects.
    MStatus status;
    MPlug plug = dagNodeFn.findPlug("template", /*wantNetworkedPlug=*/ false, &status);
    if (status == MS::kSuccess && plug.asBool())
        return false;

    // Skip invisible objects.
    plug = dagNodeFn.findPlug("visibility", /*wantNetworkedPlug=*/ false, &status);
    if (status == MS::kSuccess && plug.asBool() == false)
        return false;

    plug = dagNodeFn.findPlug("overrideVisibility", /*wantNetworkedPlug=*/ false, &status);
    if (status == MS::kSuccess && plug.asBool() == false)
        return false;

    return true;
}

bool DagNodeExporter::areObjectAndParentsRenderable(const MDagPath& path)
{
    MDagPath d(path);

    while (true)
    {
        if (isObjectRenderable(d) == false)
            return false;

        if (d.length() <= 1)
            break;

        d.pop();
    }

    return true;
}

// This code comes from Alembic's Maya AbcExport plugin.
namespace
{
    struct NodesToCheckStruct
    {
        MObject node;
        bool    checkParent;
    };
}

bool DagNodeExporter::isAnimated(MObject object, bool checkParent)
{
    MStatus stat;
    MItDependencyGraph iter(
        object,
        MFn::kInvalid,
        MItDependencyGraph::kUpstream,
        MItDependencyGraph::kDepthFirst,
        MItDependencyGraph::kPlugLevel,
        &stat);

    if (stat != MS::kSuccess)
        RENDERER_LOG_ERROR("Unable to create DG iterator");

    // MAnimUtil::isAnimated(node) will search the history of the node
    // for any animation curve nodes. It will return true for those nodes
    // that have animation curve in their history.
    // The average time complexity is O(n^2) where n is the number of history
    // nodes. But we can improve the best case by split the loop into two.
    std::vector<NodesToCheckStruct> nodesToCheckAnimCurve;

    NodesToCheckStruct nodeStruct;
    for (; !iter.isDone(); iter.next())
    {
        MObject node = iter.currentItem();

        if (
            node.hasFn(MFn::kPluginDependNode) ||
            node.hasFn(MFn::kConstraint ) ||
            node.hasFn(MFn::kPointConstraint) ||
            node.hasFn(MFn::kAimConstraint) ||
            node.hasFn(MFn::kOrientConstraint) ||
            node.hasFn(MFn::kScaleConstraint) ||
            node.hasFn(MFn::kGeometryConstraint) ||
            node.hasFn(MFn::kNormalConstraint) ||
            node.hasFn(MFn::kTangentConstraint) ||
            node.hasFn(MFn::kParentConstraint) ||
            node.hasFn(MFn::kPoleVectorConstraint) ||
            node.hasFn(MFn::kParentConstraint) ||
            node.hasFn(MFn::kTime) ||
            node.hasFn(MFn::kJoint) ||
            node.hasFn(MFn::kGeometryFilt) ||
            node.hasFn(MFn::kTweak) ||
            node.hasFn(MFn::kPolyTweak) ||
            node.hasFn(MFn::kSubdTweak) ||
            node.hasFn(MFn::kCluster) ||
            node.hasFn(MFn::kFluid) ||
            node.hasFn(MFn::kPolyBoolOp))
        {
            return true;
        }

        if (node.hasFn(MFn::kExpression))
        {
            MFnExpression fn(node, &stat);
            if (stat == MS::kSuccess && fn.isAnimated())
                return true;
        }

        if (node.hasFn(MFn::kShadingEngine))
        {
            // Skip shading nodes and don't traverse the rest of their subgraph.
            iter.prune();
        }
        else
        {
            MPlug plug = iter.thisPlug();
            MFnAttribute attr(plug.attribute(), &stat);
            bool checkNodeParent = false;

            if (stat == MS::kSuccess && attr.isWorldSpace())
                checkNodeParent = true;

            nodeStruct.node = node;
            nodeStruct.checkParent = checkParent || checkNodeParent;
            nodesToCheckAnimCurve.push_back(nodeStruct);
        }
    }

    for (size_t i = 0, e = nodesToCheckAnimCurve.size(); i < e; ++i)
    {
        if (MAnimUtil::isAnimated(nodesToCheckAnimCurve[i].node, nodesToCheckAnimCurve[i].checkParent))
            return true;
    }

    return false;
}
