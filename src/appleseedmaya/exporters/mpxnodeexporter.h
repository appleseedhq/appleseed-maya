
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

#ifndef APPLESEED_MAYA_EXPORTERS_MPXNODEEXPORTER_H
#define APPLESEED_MAYA_EXPORTERS_MPXNODEEXPORTER_H

// Boost headers.
#include "boost/shared_ptr.hpp"

// Maya headers.
#include <maya/MObject.h>
#include <maya/MObjectArray.h>
#include <maya/MString.h>

// appleseed.maya headers.
#include "appleseedmaya/appleseedsession.h"
#include "appleseedmaya/utils.h"

// Fordward declarations.
namespace renderer { class Assembly; }
namespace renderer { class Project; }
namespace renderer { class Scene; }


//
// Base class for Maya to appleseed exporters.
//

class MPxNodeExporter
  : NonCopyable
{
  public:

    // Destructor.
    virtual ~MPxNodeExporter();

    // Return the name of the appleseed entity created by this exporter.
    virtual MString appleseedName() const;

    // Create appleseed entities.
    virtual void createEntity(const AppleseedSession::Options& options) = 0;

    // Flush entities to the renderer.
    virtual void flushEntity() = 0;

  protected:

    // Constructor.
    MPxNodeExporter(
      const MObject&                object,
      renderer::Project&            project,
      AppleseedSession::SessionMode sessionMode);

    // Return the Maya dependency node.
    const MObject& node() const;

    // Return the session mode.
    AppleseedSession::SessionMode sessionMode() const;

    // Return a reference to the appleseed project.
    renderer::Project& project();

    // Return a reference to the appleseed scene.
    renderer::Scene& scene();

    // Return a reference to the appleseed main assembly.
    renderer::Assembly& mainAssembly();

  private:
    MObject                       m_object;
    AppleseedSession::SessionMode m_sessionMode;
    renderer::Project&            m_project;
    renderer::Scene&              m_scene;
    renderer::Assembly&           m_mainAssembly;
};

typedef boost::shared_ptr<MPxNodeExporter> MPxNodeExporterPtr;

#endif  // !APPLESEED_MAYA_EXPORTERS_MPXNODEEXPORTER_H
