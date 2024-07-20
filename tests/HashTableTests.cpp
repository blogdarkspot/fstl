
#include "gtest/gtest.h"
#include "hash_table_lock_free.hpp"
#include <thread>
#include <string>
#include <Vector>

class SetLockFreeTest : public testing::Test
{
  protected:
	  SetLockFreeTest() = default;
	  set_lock_free<int> _M_set;
};

TEST_F(SetLockFreeTest, one_thread_insert_test)
{
    std::vector<int> data = {1, 2, 3, 4, 5};

	for (int i = 0; i < data.size(); ++i)
	{
        _M_set.insert(data[i]);
	}
	
	EXPECT_EQ(data.size(), _M_set.size());


}

TEST_F(SetLockFreeTest, two_threads_insert_test)
{

    std::vector<int> data;

    for (int i = 0; i < 50; ++i)
    {
        data.push_back(i);
    }

    auto th1 = std::thread([&]() {
        for (int i = 0; i < data.size(); ++i)
        {
            _M_set.insert(data[i]);
        }
    });
    auto th2 = std::thread([&]() {
        for (int i = 0; i < data.size(); ++i)
        {
            _M_set.insert(data[i]);
        }
    });
    th1.join();
    th2.join();
	EXPECT_EQ(data.size(), _M_set.size());

}

TEST_F(SetLockFreeTest, find_values)
{
    std::vector<int> data = {1, 2, 3, 4, 5};
    for (const auto& v : data)
    {
        _M_set.insert(v);
    }

    for (const auto& v : data)
    {
        auto r = _M_set.find(v);
        EXPECT_NE(_M_set.end(), r);
        EXPECT_EQ(*r, v);
    }
}
