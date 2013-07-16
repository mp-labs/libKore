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

TEST( SerializationTest, SerializeBlock )
{
    MyBlock1* block = K_BLOCK_CREATE_INSTANCE( MyBlock1 );
    block->setLeInt( 123456 );
    block->setLaString( "Hello World!" );

    QByteArray buffer;
    QBuffer device( & buffer );
    device.open( QIODevice::ReadWrite );

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

    EXPECT_TRUE( block->leInt() == inflatedBlock->to< MyBlock1 >()->leInt() );

    EXPECT_TRUE( block->laString() ==
                 inflatedBlock->to< MyBlock1 >()->laString() );

    delete block;
    delete inflatedBlock;
}

TEST( SerializationTest, SerializeTree )
{
    MyLibrary* lib1 = K_BLOCK_CREATE_INSTANCE( MyLibrary );

    MyBlock1* block1 = K_BLOCK_CREATE_INSTANCE( MyBlock1 );
    block1->setLeInt( 123 );
    block1->setLaString( "Blabla" );
    block1->leCustomType().laString = "Ahahah";
    block1->leCustomType().leInt32 = 254;

    lib1->addBlock( block1 );

    MyLibrary* lib2 = K_BLOCK_CREATE_INSTANCE( MyLibrary );
    lib1->addBlock( lib2 );

    MyBlock1* block2 = K_BLOCK_CREATE_INSTANCE( MyBlock1 );
    block2->setLeInt( 456 );
    block2->setLaString( "Tududu" );
    lib2->addBlock( block2 );

    // This type is not serializable !
    MyBlock2* block3 = K_BLOCK_CREATE_INSTANCE( MyBlock2 );
    lib2->addBlock( block3 );

    ASSERT_TRUE( lib1->size() == 2 );
    ASSERT_TRUE( lib1->at( 0 )->fastInherits< MyBlock1 >() );
    EXPECT_TRUE( lib1->at< MyBlock1 >( 0 )->leInt() == 123 );
    EXPECT_TRUE( lib1->at< MyBlock1 >( 0 )->laString() == "Blabla" );
    EXPECT_TRUE( lib1->at< MyBlock1 >( 0 )->leCustomType().laString ==
                 "Ahahah" );
    EXPECT_TRUE( lib1->at< MyBlock1 >( 0 )->leCustomType().leInt32 == 254 );

    ASSERT_TRUE( lib1->at( 1 )->fastInherits< MyLibrary >() );
    ASSERT_TRUE( lib1->at< MyLibrary >( 1 )->size() == 2 );
    ASSERT_TRUE(
        lib1->at< MyLibrary >( 1 )->at( 0 )->fastInherits< MyBlock1 >() );
    ASSERT_TRUE(
        lib1->at< MyLibrary >( 1 )->at( 1 )->fastInherits< MyBlock2 >() );
    EXPECT_TRUE(
        lib1->at< MyLibrary >( 1 )->at< MyBlock1 >( 0 )->leInt() == 456 );
    EXPECT_TRUE( lib1->at< MyLibrary >( 1 )->at< MyBlock1 >( 0 )->laString() ==
                 "Tududu" );

    // Now serialize
    QByteArray buffer;
    QBuffer device( & buffer );
    device.open( QIODevice::ReadWrite );

    int err;

    KoreSerializer serializer;
    err = serializer.deflate( & device, lib1, K_NULL );
    ASSERT_TRUE( KoreSerializer::NoError == err ) << "Error code: " << err;

    // Reset...
    device.seek( 0 );

    Block* inflatedBlock;
    err = serializer.inflate( & device, & inflatedBlock, K_NULL );
    ASSERT_TRUE( KoreSerializer::NoError == err ) << "Error code: " << err;

    ASSERT_TRUE( inflatedBlock->fastInherits< MyLibrary >() );

    MyLibrary* iLib1 = inflatedBlock->to< MyLibrary >();

    ASSERT_TRUE( iLib1->size() == 2 ) << iLib1->size() << " child block(s)";
    ASSERT_TRUE( iLib1->at( 0 )->fastInherits< MyBlock1 >() );
    EXPECT_TRUE( iLib1->at< MyBlock1 >( 0 )->leInt() == 123 );
    EXPECT_TRUE( iLib1->at< MyBlock1 >( 0 )->laString() == "Blabla" );
    EXPECT_TRUE( iLib1->at< MyBlock1 >( 0 )->leCustomType().laString ==
                 "Ahahah" );
    EXPECT_TRUE( iLib1->at< MyBlock1 >( 0 )->leCustomType().leInt32 == 254 );

    ASSERT_TRUE( iLib1->at( 1 )->fastInherits< MyLibrary >() );
    EXPECT_TRUE( iLib1->at< MyLibrary >( 1 )->size() == 1 );
    ASSERT_TRUE(
        iLib1->at< MyLibrary >( 1 )->at( 0 )->fastInherits< MyBlock1 >() );
    EXPECT_TRUE(
        iLib1->at< MyLibrary >( 1 )->at< MyBlock1 >( 0 )->leInt() == 456 );
    EXPECT_TRUE( iLib1->at< MyLibrary >( 1 )->at< MyBlock1 >( 0 )->laString() ==
                 "Tududu" );

    delete lib1;
    delete iLib1;
}
