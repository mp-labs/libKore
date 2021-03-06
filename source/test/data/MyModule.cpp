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

#include "MyModule.hpp"
#include "MyCustomType.hpp"

#define K_MODULE_TYPES_LIST \
    K_MODULE_REGISTER_META_TYPE( DataTestModule::MyCustomType )

#include <plugin/ModuleMacros.hpp>
K_MODULE_IMPL

using namespace DataTestModule;

namespace
{
static bool loadModule = MyModule::StaticLoad();
}

QString MyModule::id() const
{
    return QStringLiteral( "net.mp-labs.test.DataTestModule" );
}

QString MyModule::name() const
{
    return QStringLiteral( "DataTestModule" );
}

QString MyModule::author() const
{
    return QStringLiteral( "Moving Pixel Labs" );
}

QString MyModule::url() const
{
    return QStringLiteral( "http://www.mp-labs.net" );
}

QString MyModule::version() const
{
    return QStringLiteral( "0.0.0 test" );
}

bool MyModule::StaticLoad()
{
    return moduleInstance->load();
}
