
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

#ifndef APPLESEED_MAYA_UTILS_H
#define APPLESEED_MAYA_UTILS_H

// Standard headers.
#include <cstring>

// Maya headers.
#include <maya/MString.h>

// appleseed.foundation headers.
#include "foundation/utility/autoreleaseptr.h"


//
// NonCopyable.
//

class NonCopyable
{
  protected:
    NonCopyable() {}
    ~NonCopyable() {}

  private:
    NonCopyable(const NonCopyable&);
    NonCopyable& operator=(const NonCopyable&);
};


//
// Function object class for MString less-than comparison.
// Used in maps as a compare predicate, to avoid MString <-> std::string copies.
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

template<class T>
class AppleseedEntityPtr
  : NonCopyable
{
  public:
    AppleseedEntityPtr()
      : m_ptr(0)
      , m_releaseObj(false)
    {
    }

    explicit AppleseedEntityPtr(foundation::auto_release_ptr<T> ptr)
      : m_ptr(ptr.release())
      , m_releaseObj(true)
    {
    }

    ~AppleseedEntityPtr()
    {
        if(m_releaseObj)
            m_ptr->release();
    }

    void reset()
    {
        if(m_releaseObj)
            m_ptr->release();

        m_releaseObj = false;
        m_ptr = 0;
    }

    void reset(foundation::auto_release_ptr<T> ptr)
    {
        reset();
        m_releaseObj = true;
        m_ptr = ptr.release();
    }

    foundation::auto_release_ptr<T> release()
    {
        assert(m_releaseObj);

        m_releaseObj = false;
        return foundation::auto_release_ptr<T>(m_ptr);
    }

    T& operator*() const throw()
    {
        assert(m_ptr);
        return *m_ptr;
    }
    T* operator->() const throw()
    {
        assert(m_ptr);
        return m_ptr;
    }

    T* get() const throw()
    {
        return m_ptr;
    }

  private:
    T*      m_ptr;
    bool    m_releaseObj;
};

#endif  // !APPLESEED_MAYA_UTILS_H
