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

#include "MyBlock1.hpp"
#include "MyModule.hpp"

#define K_BLOCK_SUPER_TYPE  DataTestModule::MyBlock
#define K_BLOCK_TYPE        DataTestModule::MyBlock1
#define K_BLOCK_ALLOCABLE
#include <data/BlockMacros.hpp>
K_BLOCK_IMPLEMENTATION

using namespace DataTestModule;

MyBlock1::MyBlock1( kuint64 extraFlags )
    : MyBlock( extraFlags | Serializable )
    , _leInt( 0 )
{
}

int MyBlock1::leInt() const
{
    return _leInt;
}

void MyBlock1::setLeInt( int i )
{
    _leInt = i;
}

const QString& MyBlock1::laString() const
{
    return _laString;
}

void MyBlock1::setLaString( const QString& str )
{
    _laString = str;
}

const MyCustomType& MyBlock1::leCustomType() const
{
    return _leCustomType;
}

MyCustomType& MyBlock1::leCustomType()
{
    return _leCustomType;
}

void MyBlock1::setLeCustomType( const MyCustomType& type )
{
    _leCustomType = type;
}
