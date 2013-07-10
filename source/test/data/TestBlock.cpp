
#include <gtest/gtest.h>

#include <data/Block.hpp>
#include <KoreModule.hpp>

using namespace Kore::data;

class MyBlock : public Block
{
    K_BLOCK
};

#define K_MODULE_TYPE       Kore::KoreModule
#define K_BLOCK_SUPER_TYPE  Kore::data::Block
#define K_BLOCK_TYPE        MyBlock
#define K_BLOCK_ALLOCABLE
#include <data/BlockMacros.hpp>
K_BLOCK_IMPLEMENTATION

TEST( TestBlock, Instantiate )
{
    MyBlock* b = K_BLOCK_CREATE_INSTANCE( MyBlock );
    EXPECT_TRUE( NULL != b );
    b->destroy();
}
