
#include <gtest/gtest.h>

#include <data/MetaBlock.hpp>

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
    block->destroy();
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
    lib->destroy();
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
