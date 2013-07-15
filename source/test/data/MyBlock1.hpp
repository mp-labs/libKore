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

#pragma once

#include "MyBlock.hpp"
#include "MyCustomType.hpp"

namespace DataTestModule
{

class MyBlock1 : public MyBlock
{
    Q_OBJECT

    Q_PROPERTY( int leInt MEMBER _leInt STORED ( _leInt != 0 ) )
    Q_PROPERTY( QString laString
                MEMBER _laString STORED ( ! _laString.isEmpty() ) )
    Q_PROPERTY( DataTestModule::MyCustomType leCustomType
                READ leCustomType WRITE setLeCustomType STORED true )

    K_BLOCK

public:
    MyBlock1( kuint64 extraFlags = 0 );

    int leInt() const;
    void setLeInt( int i );

    const QString& laString() const;
    void setLaString( const QString& str );

    const MyCustomType& leCustomType() const;
    void setLeCustomType( const MyCustomType& type );
    MyCustomType& leCustomType();

private:
    int          _leInt;
    QString      _laString;
    MyCustomType _leCustomType;
};

}
