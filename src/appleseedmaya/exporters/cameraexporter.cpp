
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
#include "appleseedmaya/exporters/cameraexporter.h"

// Maya headers.
#include <maya/MFnCamera.h>
#include <maya/MFnDagNode.h>
#include <maya/MPlug.h>
#include <maya/MStatus.h>

// appleseed.renderer headers.
#include "renderer/api/camera.h"
#include "renderer/api/scene.h"

// appleseed.maya headers.
#include "appleseedmaya/exporters/exporterfactory.h"


namespace asf = foundation;
namespace asr = renderer;

void CameraExporter::registerExporter()
{
    NodeExporterFactory::registerDagNodeExporter("camera", &CameraExporter::create);
}

DagNodeExporter *CameraExporter::create(const MDagPath& path, asr::Scene& scene)
{
    if(isRenderable(path))
        return new CameraExporter(path, scene);

    return 0;
}

CameraExporter::CameraExporter(const MDagPath& path, asr::Scene& scene)
  : DagNodeExporter(path, scene)
{
    MFnCamera camera(path);

    asr::CameraFactoryRegistrar cameraFactories;
    const asr::ICameraFactory *cameraFactory = 0;
    asr::ParamArray cameraParams;

    if(camera.isOrtho())
    {
        cameraFactory = cameraFactories.lookup("orthographic_camera");
    }
    else
    {
        cameraFactory = cameraFactories.lookup("pinhole_camera");
    }

    asf::auto_release_ptr<asr::Camera> cam(
        cameraFactory->create(path.fullPathName().asChar(), cameraParams));
    scene.cameras().insert(cam);
}

bool CameraExporter::isRenderable(const MDagPath& path)
{
    MFnDagNode node(path);

    MStatus status;
    MPlug p = node.findPlug("renderable", &status);

    if(status == MS::kSuccess)
    {
        bool result = true;
        p.getValue(result);
        return result;
    }

    return true;
}
