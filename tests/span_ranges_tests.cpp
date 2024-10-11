#include "span_ranges.hpp"
#include <gtest/gtest.h>

TEST(Ranges, InsertRange)
{
    std::vector<int> data = {1, 2, 3, 4, 5, 6};
    std::span<int> x = {data.begin(), 3};
    Ranges<int> range;
    range.push_back(x);
    auto ret = range.get_range(0, 2);
    EXPECT_EQ(ret.front()[0], 1);
    EXPECT_EQ(ret.front()[1], 2);
    EXPECT_EQ(ret.front().size(), 2);
    range.push_front({data.begin() + 3, 3});
    auto ret2 = range.get_range(2, 2);
    EXPECT_EQ(2, ret2.size());
    EXPECT_EQ(6, ret2[0][0]);
    EXPECT_EQ(1, ret2[1][0]);
    range.remove_range(1, 1);
    auto ret3 = range.get_range(0, 3);
    EXPECT_EQ(3, ret3.size());
    EXPECT_EQ(4, ret3[0][0]);
    EXPECT_EQ(6, ret3[1][0]);
    EXPECT_EQ(1, ret3[2][0]);
}
