
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
#include "appleseedmaya/idlejobqueue.h"

// appleseed.maya headers.
#include "appleseedmaya/logger.h"

// Maya headers.
#include <maya/MEventMessage.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include "appleseedmaya/mayaheaderscleanup.h"

// Boost headers.
#include "boost/thread/mutex.hpp"

// Standard headers.
#include <queue>

namespace
{

MCallbackId g_callbackId;

std::queue<boost::function<void ()>> g_jobQueue;
boost::mutex g_jobQueueMutex;

static void idleCallback(void* clientData)
{
    while (true)
    {
        boost::function<void ()> job;

        {
            boost::mutex::scoped_lock lock(g_jobQueueMutex);

            if (g_jobQueue.empty())
                break;

            job = g_jobQueue.front();
            g_jobQueue.pop();
        }

        job();
    }
}

} // unnamed.

namespace IdleJobQueue
{

MStatus initialize()
{
    g_callbackId = 0;
    RENDERER_LOG_INFO("Initialized idle job queue");
    return MS::kSuccess;
}

MStatus uninitialize()
{
    stop();
    RENDERER_LOG_INFO("Uninitialized idle job queue");
    return MS::kSuccess;
}

void start()
{
    if (g_callbackId == 0)
    {
        RENDERER_LOG_DEBUG("Started idle job queue");

        MStatus status;
        g_callbackId = MEventMessage::addEventCallback(
            "idle",
            &idleCallback,
            reinterpret_cast<void*>(0),
            &status);
    }
}

void stop()
{
    if (g_callbackId != 0)
    {
        RENDERER_LOG_DEBUG("Stoped idle job queue");

        MEventMessage::removeCallback(g_callbackId);
        g_callbackId = 0;

        // Perform any pending jobs.
        idleCallback(nullptr);
        assert(g_jobQueue.empty());
    }
}

void pushJob(boost::function<void ()> job)
{
    assert(job);
    assert(g_callbackId != 0);

    boost::mutex::scoped_lock lock(g_jobQueueMutex);
    g_jobQueue.push(job);
}

} // IdleJobQueue
