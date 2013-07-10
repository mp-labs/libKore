/*
 * Copyright (c) 2013, Moving Pixel Labs (http://www.mp-labs.net)
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the Moving Pixel Labs nor the names of its
 *      contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL MOVING PIXEL LABS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <parallel/TaskletRunner.hpp>
#include <parallel/Tasklet.hpp>
using namespace Kore::parallel;

TaskletRunner::~TaskletRunner()
{
}

void TaskletRunner::start( Tasklet* tasklet ) const
{
    tasklet->runnerStarted();
}

void TaskletRunner::cancel( Tasklet* tasklet ) const
{
    tasklet->runnerCanceled();
}

void TaskletRunner::fail( Tasklet* tasklet ) const
{
    tasklet->runnerFailed();
}

void TaskletRunner::complete( Tasklet* tasklet ) const
{
    tasklet->runnerCompleted();
}

void TaskletRunner::progress( Tasklet* tasklet, const QString& message ) const
{
    tasklet->runnerProgress( message );
}

void TaskletRunner::progress( Tasklet* tasklet,
                              kuint64 progress, kuint64 total ) const
{
    tasklet->runnerProgress( progress, total );
}

kbool TaskletRunner::keepRunning( Tasklet* tasklet ) const
{
    return tasklet->keepRunning();
}