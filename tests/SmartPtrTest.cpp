#include <gtest/gtest.h>
#include "smart_ptr.hpp"

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
