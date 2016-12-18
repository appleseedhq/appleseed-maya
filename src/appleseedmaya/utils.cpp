
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
#include "appleseedmaya/utils.h"

// Standard headers.
#include <algorithm>
#include <list>

// Boost headers.
#include "boost/thread/locks.hpp"
#include "boost/thread/mutex.hpp"

// Maya headers.
#include <maya/MEventMessage.h>
#include <maya/MSelectionList.h>

namespace
{

struct CallbackList
{
    struct CallbackInfo
    {
        CallbackList*           m_callbackList;
        MCallbackId             m_callbackId;
        boost::function<void()> m_callback;
    };

    struct MatchCallbackId
    {
        MatchCallbackId(const MCallbackId cid)
          : m_id(cid)
        {
        }

        bool operator()(const CallbackInfo& c) const
        {
            return c.m_callbackId == m_id;
        }

        MCallbackId m_id;
    };

    MStatus addCallback(boost::function<void()> callback)
    {
        assert(callback);

        boost::lock_guard<boost::mutex> lock(m_mutex);

        m_callbacks.push_back(CallbackInfo());
        CallbackInfo *cinfo = &m_callbacks.back();
        cinfo->m_callbackList = this;
        cinfo->m_callback = callback;

        MStatus status;
        cinfo->m_callbackId = MEventMessage::addEventCallback(
            "idle", &CallbackList::executeCallback, cinfo, &status);

        if(!status)
            m_callbacks.pop_back();

        return status;
    }

    static void executeCallback(void *clientData)
    {
        // Execute the callback.
        CallbackInfo *cinfo = reinterpret_cast<CallbackInfo*>(clientData);

        assert(cinfo->m_callback);
        cinfo->m_callback();

        MEventMessage::removeCallback(cinfo->m_callbackId);

        // Remove the callback.
        {
            boost::lock_guard<boost::mutex> lock(cinfo->m_callbackList->m_mutex);

            std::list<CallbackInfo>& clist = cinfo->m_callbackList->m_callbacks;
            std::list<CallbackInfo>::iterator it = std::find_if(
                clist.begin(), clist.end(), MatchCallbackId(cinfo->m_callbackId));
            clist.erase(it);
        }
    }

    std::list<CallbackInfo> m_callbacks;
    boost::mutex            m_mutex;
};

CallbackList gCallbackList;

} // unnamed

MStatus executeInMainThread(boost::function<void()> f)
{
    return gCallbackList.addCallback(f);
}

MStatus getDependencyNodeByName(const MString& name, MObject& node)
{
    MSelectionList selList;
    selList.add(name);

    if(selList.isEmpty())
        return MS::kFailure;

    return selList.getDependNode(0, node);
}

MStatus getDagPathByName(const MString& name, MDagPath& dag)
{
    MSelectionList selList;
    selList.add(name);

    if(selList.isEmpty())
        return MS::kFailure;

    return selList.getDagPath(0, dag);
}
