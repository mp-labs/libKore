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

#include <gtest/gtest.h>

#include <data/MetaBlock.hpp>
#include <data/Block.hpp>

#include "MyBlock.hpp"
#include "MyBlock1.hpp"
#include "MyBlock2.hpp"
#include "MyLibrary.hpp"
#include "MyModule.hpp"

using namespace DataTestModule;
using namespace Kore::data;

TEST( BlockTest, MetaInstantiateBlock )
{
    MyBlock* block = K_BLOCK_CREATE_INSTANCE( MyBlock );
    EXPECT_TRUE( NULL != block );
    delete block;
}

TEST( BlockTest, MetaBlockMetaObject )
{
    MyBlock block( Block::Static );
    EXPECT_TRUE( block.metaObject() == block.metaBlock()->blockMetaObject() );
    EXPECT_TRUE( & MyBlock::staticMetaObject ==
                 MyBlock::StaticMetaBlock()->blockMetaObject() );
    EXPECT_TRUE( block.metaBlock() == MyBlock::StaticMetaBlock() );
}

TEST( BlockTest, FastInherits )
{
    MyBlock block( Block::Static );
    MyBlock1 block1( Block::Static );
    MyBlock2 block2( Block::Static );

    EXPECT_TRUE( block1.fastInherits< MyBlock >() );
    EXPECT_TRUE( block1.fastInherits( block.metaBlock() ) );

    EXPECT_TRUE( block2.fastInherits< MyBlock >() );
    EXPECT_TRUE( block2.fastInherits( block.metaBlock() ) );

    EXPECT_FALSE( block1.fastInherits< MyBlock2 >() );
    EXPECT_FALSE( block1.fastInherits( block2.metaBlock() ) );

    EXPECT_FALSE( block2.fastInherits< MyBlock1 >() );
    EXPECT_FALSE( block2.fastInherits( block1.metaBlock() ) );

    EXPECT_FALSE( block.fastInherits< MyBlock1 >() );
    EXPECT_FALSE( block.fastInherits( block1.metaBlock() ) );

    EXPECT_FALSE( block.fastInherits< MyBlock2 >() );
    EXPECT_FALSE( block.fastInherits( block2.metaBlock() ) );

}

TEST( LibraryTest, MetaInstantiateLibrary )
{
    MyLibrary* lib = K_BLOCK_CREATE_INSTANCE( MyLibrary );
    EXPECT_TRUE( NULL != lib );
    delete lib;
}

TEST( LibraryTest, AddBlock )
{
    MyLibrary lib( Block::Static );
    MyBlock block( Block::Static );
    EXPECT_FALSE( block.hasParent() );
    lib.addBlock( & block );
    EXPECT_TRUE( lib.size() == 1 );
    EXPECT_TRUE( block.index() == 0 );
    EXPECT_TRUE( block.hasParent() );
}

TEST( LibraryTest, AutoBlockRemoval )
{
    MyLibrary lib( Block::Static );
    {
        MyBlock block( Block::Static );
        lib.addBlock( & block );
        EXPECT_TRUE( lib.size() == 1 );
    }
    EXPECT_TRUE( lib.size() == 0 );
}

TEST( LibraryTest, RemoveBlock )
{
    MyLibrary lib( Block::Static );
    MyBlock block1( Block::Static );
    MyBlock block2( Block::Static );

    lib.addBlock( & block1 );
    lib.addBlock( & block2 );
    EXPECT_TRUE( lib.size() == 2 );
    lib.removeBlock( & block1 );
    EXPECT_TRUE( lib.size() == 1 );
    EXPECT_TRUE( -1 == block1.index() );
    EXPECT_TRUE( 0 == block2.index() );
}

TEST( LibraryTest, SwapBlocks )
{
    MyLibrary lib( Block::Static );
    MyBlock block1( Block::Static );
    MyBlock block2( Block::Static );
    lib.addBlock( & block1 );
    lib.addBlock( & block2 );
    EXPECT_TRUE( lib.size() == 2 );
    EXPECT_TRUE( block1.index() == 0 );
    EXPECT_TRUE( block2.index() == 1 );

    lib.swapBlocks( & block1, & block2 );
    EXPECT_TRUE( block1.index() == 1 );
    EXPECT_TRUE( block2.index() == 0 );

    lib.swapBlocks( & block1, & block2 );
    EXPECT_TRUE( block1.index() == 0 );
    EXPECT_TRUE( block2.index() == 1 );
}

TEST( CustomTypeTest, ToAndFromVariant )
{
    MyCustomType myType;
    myType.laString = "Hello !";
    myType.leInt32 = 55;

    QVariant v = QVariant::fromValue( myType );
    EXPECT_TRUE( v.isValid() );
    EXPECT_TRUE( v.userType() == qRegisterMetaType< MyCustomType >() );

    MyCustomType myType2 = v.value< MyCustomType >();
    EXPECT_TRUE( myType.laString == myType2.laString );
    EXPECT_TRUE( myType.leInt32 == myType2.leInt32 );
}
