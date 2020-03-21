
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

#pragma once

// Forward declaration header.
#include "dagnodeexporterfwd.h"

// appleseed-maya headers.
#include "appleseedmaya/appleseedsession.h"
#include "appleseedmaya/utils.h"

// Build options header.
#include "foundation/core/buildoptions.h"

// appleseed.renderer headers.
#include "renderer/api/utility.h"

// appleseed.foundation headers.
#include "foundation/core/concepts/noncopyable.h"
#include "foundation/math/aabb.h"
#include "foundation/math/matrix.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MDagPath.h>
#include <maya/MMatrix.h>
#include <maya/MObject.h>
#include <maya/MObjectArray.h>
#include <maya/MString.h>
#include "appleseedmaya/_endmayaheaders.h"

// Forward declarations.
namespace renderer { class Assembly; }
namespace renderer { class Project; }
namespace renderer { class Scene; }
class MotionBlurSampleTimes;

//
// Base class for exporting Maya dag nodes to appleseed projects.
//

class DagNodeExporter
  : public foundation::NonCopyable
{
  public:
    // Destructor.
    virtual ~DagNodeExporter();

    // Return the Maya dependency node.
    MObject node() const;

    // Return the Maya dag path.
    const MDagPath& dagPath() const;

    // Return the name of the entity in the appleseed project.
    MString appleseedName() const;

    // Return true if the entity created by this exporter can be motion blurred.
    virtual bool supportsMotionBlur() const;

    // Create any extra exporter needed by this exporter (shading engines, ...).
    virtual void createExporters(const AppleseedSession::IExporterFactory& exporter_factory);

    // Create appleseed entities.
    virtual void createEntities(
        const AppleseedSession::Options&                options,
        const AppleseedSession::MotionBlurSampleTimes&  motionBlurSampleTimes) = 0;

    // Motion blur.
    virtual void exportCameraMotionStep(float time);
    virtual void exportTransformMotionStep(float time);
    virtual void exportShapeMotionStep(float time);

    // Flush entities to the renderer.
    virtual void flushEntities() = 0;

    // Bounds.
    virtual foundation::AABB3d boundingBox() const;

  protected:
    // Constructor.
    DagNodeExporter(
      const MDagPath&                                   path,
      renderer::Project&                                project,
      AppleseedSession::SessionMode                     sessionMode);

    // Return the session mode.
    AppleseedSession::SessionMode sessionMode() const;

    // Return a reference to the appleseed project.
    renderer::Project& project();

    // Return a reference to the appleseed scene.
    renderer::Scene& scene();

    // Return a reference to the appleseed main assembly.
    renderer::Assembly& mainAssembly();

    // Convert a Maya matrix to an appleseed matrix.
    foundation::Matrix4d convert(const MMatrix& m) const;

    // Add appleseed visibility attributes to the ParamArray.
    void addVisibilityAttributesToParams(renderer::ParamArray& params);

    // Return true if an object is renderable.
    static bool isObjectRenderable(const MDagPath& path);

    // Return true if an object and all its parents are renderable.
    static bool areObjectAndParentsRenderable(const MDagPath& path);

    // Return true if an object is animated.
    static bool isAnimated(MObject object, bool checkParent = false);

    // Return the object space bounding box.
    static foundation::AABB3d objectSpaceBoundingBox(const MDagPath& path);


  private:
    MDagPath                      m_path;
    AppleseedSession::SessionMode m_sessionMode;
    renderer::Project&            m_project;
    renderer::Scene&              m_scene;
    renderer::Assembly&           m_mainAssembly;
};

