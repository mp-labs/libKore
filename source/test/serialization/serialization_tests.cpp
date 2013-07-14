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

#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtCore/QTemporaryFile>

#include <gtest/gtest.h>

#include <data/MetaBlock.hpp>

#include <serialization/KoreSerializer.hpp>

#include "../data/MyBlock.hpp"
#include "../data/MyBlock1.hpp"
#include "../data/MyBlock2.hpp"
#include "../data/MyLibrary.hpp"

using namespace DataTestModule;
using namespace Kore::data;
using namespace Kore::serialization;

TEST( SerializationTest, SerializeSimpleBlock )
{
    MyBlock1* block = K_BLOCK_CREATE_INSTANCE( MyBlock1 );
    block->setLeInt( 123456 );
    block->setLaString( "Hello World!" );

    /*QByteArray buffer;
    QBuffer device( & buffer );
    device.open( QIODevice::ReadWrite | QIODevice::Truncate );*/

    QTemporaryFile device( "./Kore_serialization_test_file" );
    device.setAutoRemove( false );
    device.open();

    int err;

    KoreSerializer serializer;
    err = serializer.deflate( & device, block, K_NULL );
    ASSERT_TRUE( KoreSerializer::NoError == err ) << "Error code: " << err;

    // Reset...
    device.seek( 0 );

    Block* inflatedBlock;
    err = serializer.inflate( & device, & inflatedBlock, K_NULL );

    ASSERT_TRUE( KoreSerializer::NoError == err ) << "Error code: " << err;
    ASSERT_TRUE( K_NULL != inflatedBlock );

    ASSERT_TRUE( inflatedBlock->fastInherits< MyBlock1 >() );

    EXPECT_TRUE( block->leInt() ==
                 static_cast< MyBlock1* >( inflatedBlock )->leInt() );

    EXPECT_TRUE( block->laString() ==
                 static_cast< MyBlock1* >( inflatedBlock )->laString() );
}
