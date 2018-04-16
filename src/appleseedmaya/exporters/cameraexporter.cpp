
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2016-2018 Esteban Tovagliari, The appleseedhq Organization
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
#include "cameraexporter.h"

// appleseed-maya headers.
#include "appleseedmaya/attributeutils.h"
#include "appleseedmaya/exporters/exporterfactory.h"

// appleseed.renderer headers.
#include "renderer/api/scene.h"

// appleseed.foundation headers
#include "foundation/math/vector.h"
#include "foundation/utility/iostreamop.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MFnCamera.h>
#include <maya/MFnDagNode.h>
#include <maya/MPlug.h>
#include <maya/MStatus.h>
#include "appleseedmaya/_endmayaheaders.h"

namespace asf = foundation;
namespace asr = renderer;

void CameraExporter::registerExporter()
{
    NodeExporterFactory::registerDagNodeExporter("camera", &CameraExporter::create);
}

DagNodeExporter* CameraExporter::create(
    const MDagPath&                                 path,
    asr::Project&                                   project,
    AppleseedSession::SessionMode                   sessionMode)
{
    if (isRenderable(path))
        return new CameraExporter(path, project, sessionMode);

    return nullptr;
}

CameraExporter::CameraExporter(
    const MDagPath&                                 path,
    asr::Project&                                   project,
    AppleseedSession::SessionMode                   sessionMode)
  : DagNodeExporter(path, project, sessionMode)
{
}

CameraExporter::~CameraExporter()
{
    if (sessionMode() == AppleseedSession::ProgressiveRenderSession)
        scene().cameras().remove(m_camera.get());
}

void CameraExporter::createEntities(
    const AppleseedSession::Options&                options,
    const AppleseedSession::MotionBlurSampleTimes&  motionBlurSampleTimes)
{
    MFnCamera camera(dagPath());

    const asr::CameraFactoryRegistrar& cameraFactories =
        project().get_factory_registrar<asr::Camera>();

    const asr::ICameraFactory* cameraFactory = nullptr;
    asr::ParamArray cameraParams;

    if (camera.isOrtho())
    {
        cameraFactory = cameraFactories.lookup("orthographic_camera");
        // TODO: fetch ortho camera params here.
    }
    else
    {
        const bool dofEnabled = false;

        if (dofEnabled)
            cameraFactory = cameraFactories.lookup("thin_lens_camera");
        else
            cameraFactory = cameraFactories.lookup("pinhole_camera");

        // Maya's aperture is given in inches so convert to cm and then to meters.
        double horizontalFilmAperture = camera.horizontalFilmAperture() * 2.54 * 0.01;
        double verticalFilmAperture = camera.verticalFilmAperture() * 2.54 * 0.01;

        const double imageAspect = static_cast<double>(options.m_width) / options.m_height;

        // Handle film fits.
        // Reference: http://around-the-corner.typepad.com/adn/2012/11/maya-stereoscopic.html
        MFnCamera::FilmFit filmFit = camera.filmFit();
        const double filmAspect = horizontalFilmAperture / verticalFilmAperture;

        if (filmFit == MFnCamera::kFillFilmFit)
        {
            filmFit = filmAspect < imageAspect
                ? MFnCamera::kHorizontalFilmFit
                : MFnCamera::kVerticalFilmFit;
        }
        else if (filmFit == MFnCamera::kOverscanFilmFit)
        {
            filmFit = filmAspect < imageAspect
                ? MFnCamera::kVerticalFilmFit
                : MFnCamera::kHorizontalFilmFit;
        }

        if (filmFit == MFnCamera::kHorizontalFilmFit)
            verticalFilmAperture = horizontalFilmAperture / imageAspect;
        else // if (filmFit == MFnCamera::kVerticalFilmFit)
            horizontalFilmAperture = verticalFilmAperture * imageAspect;

        cameraParams.insert(
            "film_dimensions",
            asf::Vector2d(horizontalFilmAperture, verticalFilmAperture));

        // Maya's aperture is given in mm so we convert it to meters.
        cameraParams.insert("focal_length", camera.focalLength() * 0.001);

        if (dofEnabled)
        {
            // TODO: extract dof params here...
        }
    }

    m_camera = cameraFactory->create(appleseedName().asChar(), cameraParams);
}

void CameraExporter::exportCameraMotionStep(float time)
{
    asf::Matrix4d m = convert(dagPath().inclusiveMatrix());
    asf::Matrix4d invM = convert(dagPath().inclusiveMatrixInverse());
    asf::Transformd xform(m, invM);
    m_camera->transform_sequence().set_transform(time, xform);
}

void CameraExporter::flushEntities()
{
    m_camera->transform_sequence().optimize();
    scene().cameras().insert(m_camera.release());
}

bool CameraExporter::isRenderable(const MDagPath& path)
{
    bool isRenderable = false;
    AttributeUtils::get(path.node(), "renderable", isRenderable);
    return isRenderable;
}
