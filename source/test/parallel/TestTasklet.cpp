
#include <gtest/gtest.h>

#include <parallel/Tasklet.hpp>
#include <KoreModule.hpp>

using namespace Kore::parallel;

class MyTasklet : public Tasklet
{
    K_TASKLET
};

#define K_TASKLET_TYPE MyTasklet
#include <parallel/TaskletMacros.hpp>
K_TASKLET_IMPLEMENTATION

TEST( TestBlock, Instantiate )
{
    MyTasklet* b = K_BLOCK_CREATE_INSTANCE( MyTasklet );
    EXPECT_TRUE( NULL != b );
    b->destroy();
}
