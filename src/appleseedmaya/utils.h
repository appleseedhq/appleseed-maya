
//
// This source file is part of appleseed.
// Visit https://appleseedhq.net/ for additional information and resources.
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

#ifndef APPLESEED_MAYA_UTILS_H
#define APPLESEED_MAYA_UTILS_H

// appleseed.foundation headers.
#include "foundation/core/concepts/noncopyable.h"
#include "foundation/utility/autoreleaseptr.h"
#include "foundation/utility/string.h"

// Maya headers.
#include "appleseedmaya/_beginmayaheaders.h"
#include <maya/MComputation.h>
#include <maya/MDagPath.h>
#include <maya/MObject.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include "appleseedmaya/_endmayaheaders.h"

// Standard headers.
#include <cstring>
#include <memory>
#include <string>

//
// MStringCompareLess
//
//  Function object class for MString less-than comparison.
//  Used in maps as a compare predicate, to avoid MString <-> std::string conversions.
//

struct MStringCompareLess
{
    bool operator()(const MString& a, const MString& b) const
    {
        return strcmp(a.asChar(), b.asChar()) < 0;
    }
};

//
// AppleseedEntityPtr.
//
//  Smart ptr that holds an appleseed entity and keeps track of ownership.
//

template <typename T>
class AppleseedEntityPtr
  : public foundation::NonCopyable
{
  public:
    AppleseedEntityPtr()
      : m_ptr(nullptr)
      , m_releaseObj(false)
    {
    }

    AppleseedEntityPtr(foundation::auto_release_ptr<T> ptr)
      : m_ptr(ptr.release())
      , m_releaseObj(true)
    {
    }

    ~AppleseedEntityPtr()
    {
        if (m_releaseObj)
            m_ptr->release();
    }

    void reset()
    {
        if (m_releaseObj)
            m_ptr->release();

        m_releaseObj = false;
        m_ptr = nullptr;
    }

    void reset(foundation::auto_release_ptr<T> ptr)
    {
        reset();
        m_releaseObj = true;
        m_ptr = ptr.release();
    }

    template <typename U>
    void reset(foundation::auto_release_ptr<U> ptr)
    {
        reset();
        m_releaseObj = true;
        m_ptr = static_cast<T*>(ptr.release());
    }

    AppleseedEntityPtr& operator=(foundation::auto_release_ptr<T> ptr)
    {
        reset(ptr);
        return *this;
    }

    foundation::auto_release_ptr<T> release()
    {
        assert(m_releaseObj);

        m_releaseObj = false;
        return foundation::auto_release_ptr<T>(m_ptr);
    }

    template <typename U>
    foundation::auto_release_ptr<U> releaseAs()
    {
        assert(m_releaseObj);

        m_releaseObj = false;
        return foundation::auto_release_ptr<U>(m_ptr);
    }

    T& operator*() const
    {
        assert(m_ptr);
        return *m_ptr;
    }

    T* operator->() const
    {
        assert(m_ptr);
        return m_ptr;
    }

    T* get() const
    {
        return m_ptr;
    }

  private:
    T*      m_ptr;
    bool    m_releaseObj;
};

// Insert an appleseed entity into a container with an unique name.
template <typename Container, typename T>
void insertEntityWithUniqueName(
    Container&              container,
    AppleseedEntityPtr<T>&  entity)
{
    std::string name = entity->get_name();

    if (container.get_by_name(name.c_str()) == nullptr)
    {
        container.insert(entity.release());
        return;
    }

    std::string pattern = name + "_#";
    size_t i = 2;

    while (true)
    {
        std::string new_name = foundation::get_numbered_string(pattern, i++);

        if (container.get_by_name(new_name.c_str()) == nullptr)
        {
            entity->set_name(new_name.c_str());
            container.insert(entity.release());
            break;
        }
    }
}

// Get Maya dependency nodes and dag paths by name.
MStatus getDependencyNodeByName(const MString& name, MObject& object);
MStatus getDagPathByName(const MString& name, MDagPath& dag);

//
// Simple wrapper around MComputation
//

class Computation
  : public foundation::NonCopyable
{
  public:
    // Create a computation wrapper.
    static std::shared_ptr<Computation> create();

    // Destructor.
    ~Computation();

    // Check if the user requested to interrupt the computation.
    bool isInterruptRequested();

    // Throw AbortRequested if the user requested to interrupt the computation.
    void thowIfInterruptRequested();

  private:
    Computation();

    MComputation m_computation;
};

typedef std::shared_ptr<Computation> ComputationPtr;

// Convert image coordinates from Y down to Y up.
template <typename T>
inline T flip_pixel_coordinate(const T size, const T x)
{
    return size - x - 1;
}

// Convert image coordinates from Y down to Y up.
template <typename T>
inline void flip_pixel_interval(const T size, T& xmin, T& xmax)
{
    T tmp = xmax;
    xmax = flip_pixel_coordinate(size, xmin);
    xmin = flip_pixel_coordinate(size, tmp);
}

//
// Deleter that calls delete[] for use with C++11 shared_ptr.
// TODO: check that we really need this.
//

template <typename T>
struct ArrayDeleter
{
    void operator()(const T* p)
    {
        delete[] p;
    }
};

#endif  // !APPLESEED_MAYA_UTILS_H
