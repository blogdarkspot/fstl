#include <gtest/gtest.h>
#include "memory.hpp"

TEST(SmartPtr, Test1)
{
    memory::SmartPtr<int> ptr;
    *ptr = 1;
    EXPECT_EQ(*ptr, 1);
}

class WrapPtrTest : public testing::Test
{
  protected:
    memory::WrapPtr<int> _M_obj;
};

TEST_F(WrapPtrTest, RefCountTest)
{
    {
        auto ptr = _M_obj.try_acquire();
        EXPECT_FALSE(ptr);
    }

    {
        _M_obj.reset(new int(1));
        auto ptr = _M_obj.try_acquire();
        EXPECT_TRUE(ptr);
        EXPECT_EQ(*ptr, 1);
        _M_obj.release(); //release try_acquire();
        _M_obj.release(); //reset
    }

    {
        auto ptr = _M_obj.try_acquire();
        EXPECT_FALSE(ptr);
    }
}
