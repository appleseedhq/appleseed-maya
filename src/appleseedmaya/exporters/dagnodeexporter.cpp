
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016-2017 Esteban Tovagliari, The appleseedhq Organization
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

// appleseed.renderer headers.
#include "renderer/api/project.h"
#include "renderer/api/scene.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MAnimUtil.h>
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

void DagNodeExporter::collectMotionBlurSteps(MotionBlurTimes& motionTimes) const
{
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

    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
            result(i, j) = m[j][i];
    }

    return result;
}

void DagNodeExporter::addVisibilityAttributesToParams(asr::ParamArray& params)
{
    asf::Dictionary visFlags;

    bool flag = true;
    if(AttributeUtils::get(node(), "asVisibilityCamera", flag))
        visFlags.insert("camera", flag);

    flag = true;
    if (AttributeUtils::get(node(), "asVisibilityLight", flag))
        visFlags.insert("light", flag);

    flag = true;
    if (AttributeUtils::get(node(), "asVisibilityShadow", flag))
        visFlags.insert("shadow", flag);

    flag = true;
    if (AttributeUtils::get(node(), "asVisibilityDiffuse", flag))
        visFlags.insert("diffuse", flag);

    flag = true;
    if (AttributeUtils::get(node(), "asVisibilitySpecular", flag))
        visFlags.insert("specular", flag);

    flag = true;
    if (AttributeUtils::get(node(), "asVisibilityGlossy", flag))
        visFlags.insert("glossy", flag);

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
    MPlug plug = dagNodeFn.findPlug("template", false, &status);
    if (status == MS::kSuccess && plug.asBool())
        return false;

    // Skip invisible objects.
    plug = dagNodeFn.findPlug("visibility", false, &status);
    if (status == MS::kSuccess && plug.asBool() == false)
        return false;

    plug = dagNodeFn.findPlug("overrideVisibility", false, &status);
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

// This comes from Alembic's Maya AbcExport plugin.
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
    MItDependencyGraph iter(object, MFn::kInvalid,
        MItDependencyGraph::kUpstream,
        MItDependencyGraph::kDepthFirst,
        MItDependencyGraph::kPlugLevel,
        &stat);

    if (stat!= MS::kSuccess)
        MGlobal::displayError("Unable to create DG iterator ");

    // MAnimUtil::isAnimated(node) will search the history of the node
    // for any animation curve nodes. It will return true for those nodes
    // that have animation curve in their history.
    // The average time complexity is O(n^2) where n is the number of history
    // nodes. But we can improve the best case by split the loop into two.
    std::vector<NodesToCheckStruct> nodesToCheckAnimCurve;

    NodesToCheckStruct nodeStruct;
    for (; !iter.isDone(); iter.next())
    {
        MObject node = iter.thisNode();

        if (node.hasFn(MFn::kPluginDependNode) ||
                node.hasFn( MFn::kConstraint ) ||
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

        // skip shading nodes
        if (!node.hasFn(MFn::kShadingEngine))
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
        else
        {
            // and don't traverse the rest of their subgraph
            iter.prune();
        }
    }

    for (size_t i = 0; i < nodesToCheckAnimCurve.size(); ++i)
    {
        if (MAnimUtil::isAnimated(nodesToCheckAnimCurve[i].node, nodesToCheckAnimCurve[i].checkParent))
            return true;
    }

    return false;
}
