#include <gtest/gtest.h>
#include "list.hpp"

class LinkedListTest : public testing::Test {
protected:
	LinkedListTest() {
		for (int i = 0; i < sz; ++i)
		{
			_M_ll1.push_back(_result[i]);
		}
	}
	lf::List<int> _M_ll0;
	lf::List<int> _M_ll1;
	int _result[5] = { 1, 2, 3, 4, 5 };
	const std::size_t sz = 5;
};

TEST_F(LinkedListTest, CreateEmptyListAndCheckIterators)
{
	auto begin = _M_ll0.begin();
	auto end = _M_ll0.end();
	EXPECT_TRUE(begin == end);
}

TEST_F(LinkedListTest, AppendOneValueAndCheckInterators)
{
	_M_ll0.push_back(2);
	auto begin = _M_ll0.begin();
	auto end = _M_ll0.end();
	EXPECT_TRUE(begin != end);
	++begin;
	EXPECT_TRUE(begin == end);
}

TEST_F(LinkedListTest, IterateOverList)
{
	for (auto i = 0; i < sz; ++i)
	{
		_M_ll0.push_back(_result[i]);
	}
	int i = 0;
	for (auto v : _M_ll0)
	{
		EXPECT_EQ(v, _result[i]);
		++i;
	}
}

TEST_F(LinkedListTest, Remove)
{
	for (int i = 0; i < sz; ++i)
	{
		_M_ll1.remove(_result[i]);
	}
	EXPECT_EQ(_M_ll1.size(), 0x00);
	auto begin = _M_ll1.begin();
	auto end = _M_ll1.end();
	EXPECT_TRUE(begin == end);
}