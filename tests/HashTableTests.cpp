
#include "gtest/gtest.h"
#include "hash_table_lock_free.hpp"
#include <thread>
#include <string>
#include <vector>

class HashTableTest : public testing::Test
{
  protected:
    HashTableTest()
    {
    }

    lf::HashTable<int, float> _M_hash;
};

TEST_F(HashTableTest, CreateHash)
{
    EXPECT_EQ(0x00, _M_hash.size());
    _M_hash.insert({1, 1.0f});
}

class VecTableTest : public testing::Test
{
  protected:
    VecTableTest() : _M_size(1 << 5), _M_table(_M_size)
    {
    }

    size_t _M_size;
    lf::VecTable<int> _M_table;
};

TEST_F(VecTableTest, InsertElements)
{
    auto &ps0 = _M_table[0];
    EXPECT_EQ(0, ps0);
    ps0 = 1;
    auto value = _M_table[0];
    EXPECT_EQ(1, value);
}

TEST_F(VecTableTest, Insert10Elements)
{
    auto values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    size_t count = 0;
    for (auto value : values)
    {
        _M_table[count++] = value;
    }

    count = 0;

    for (auto value : values)
    {
        EXPECT_EQ(value, _M_table[count++]);
    }
}

class SetLockFreeTest : public testing::Test
{
  protected:
	  SetLockFreeTest() = default;
	  lf::set<int> _M_set;
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
    for (const auto v : data)
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


TEST_F(SetLockFreeTest, erase_values)
{
    std::vector<int> data = {1, 2, 3, 4, 5};
    for (const auto& v : data)
    {
        _M_set.insert(v);
    }
    auto size = data.size();
    for (const auto& v : data)
    {
        auto r = _M_set.erase(v);
        auto it = _M_set.find(v);
        EXPECT_EQ(_M_set.end(), it);
        EXPECT_EQ(--size, _M_set.size());
    }
}

