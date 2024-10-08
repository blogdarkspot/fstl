#include "order_statistics_weight.hpp"
#include <vector>
#include <gtest/gtest.h>



TEST(OrderCache, InsertNewOrder)
{
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8};
    auto sp = std::span<int>(v.begin(), v.end());
    Ranges range;
    range.push_back(sp);
    {
        auto x = range.get_range(0, 3);
        auto it = x.begin();
        EXPECT_EQ((*it)[0], 1);
        EXPECT_EQ((*it)[1], 2);
        EXPECT_EQ((*it)[2], 3);
    }

    {
        auto x = range.get_range(2, 3);
        auto it = x.begin();
        EXPECT_EQ((*it)[0], 3);
        EXPECT_EQ((*it)[1], 4);
        EXPECT_EQ((*it)[2], 5);
    }

    {
        auto x = range.get_range(0, 8);
        auto it = x.begin();
        EXPECT_EQ((*it)[0], 1);
        EXPECT_EQ((*it)[1], 2);
        EXPECT_EQ((*it)[2], 3);
        EXPECT_EQ((*it)[3], 4);
        EXPECT_EQ((*it)[4], 5);
        EXPECT_EQ((*it)[5], 6);
        EXPECT_EQ((*it)[6], 7);
        EXPECT_EQ((*it)[7], 8);
    }
    
}

TEST(OrderCache, InsertNewOrder2)
{
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8};
    Ranges range;

    auto p1 = std::span<int>(v.begin(), 3);
    auto p2 = std::span<int>(v.begin() + 3, 3);
    auto p3 = std::span<int>(v.begin() + 6, v.end());

    range.push_back(p1);
    range.push_back(p2);
    range.push_back(p3);

    {
        auto x = range.get_range(0, 3);
        auto it = x.begin();
        EXPECT_EQ((*it)[0], 1);
        EXPECT_EQ((*it)[1], 2);
        EXPECT_EQ((*it)[2], 3);
    }

    {
        auto x = range.get_range(2, 3);
        auto it = x.begin();
        EXPECT_EQ((*it)[0], 3);
        ++it;
        EXPECT_EQ((*it)[0], 4);
        EXPECT_EQ((*it)[1], 5);
    }

    {
        auto x = range.get_range(2, 5);
        auto it = x.begin();
        EXPECT_EQ((*it)[0], 3);
        ++it;
        EXPECT_EQ((*it)[0], 4);
        EXPECT_EQ((*it)[1], 5);
        EXPECT_EQ((*it)[2], 6);
        ++it;
        EXPECT_EQ((*it)[0], 7);
    }

    {
        auto x = range.get_range(0, 8);
        auto it = x.begin();
        EXPECT_EQ((*it)[0], 1);
        EXPECT_EQ((*it)[1], 2);
        EXPECT_EQ((*it)[2], 3);
        ++it;
        EXPECT_EQ((*it)[0], 4);
        EXPECT_EQ((*it)[1], 5);
        EXPECT_EQ((*it)[2], 6);
        ++it;
        EXPECT_EQ((*it)[0], 7);
        EXPECT_EQ((*it)[1], 8);
    }
    
}

TEST(OrderCache, Remove)
{
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8};
    Ranges range;

    auto p1 = std::span<int>(v.begin(), 3);
    auto p2 = std::span<int>(v.begin() + 3, 3);
    auto p3 = std::span<int>(v.begin() + 6, v.end());

    range.push_back(p1);
    range.push_back(p2);
    range.push_back(p3);

    {
        range.remove_range(0, 1);
        auto x = range.get_range(0, 7);
        auto it = x.begin();
        EXPECT_EQ((*it)[0], 2);
        EXPECT_EQ((*it)[1], 3);
        ++it;
        EXPECT_EQ((*it)[0], 4);
        EXPECT_EQ((*it)[1], 5);
        EXPECT_EQ((*it)[2], 6);
        ++it;
        EXPECT_EQ((*it)[0], 7);
        EXPECT_EQ((*it)[1], 8);
        EXPECT_EQ(range.size(), 7);
    }

    {
        range.remove_range(0, 2);
        auto x = range.get_range(0, 5);
        auto it = x.begin();
        EXPECT_EQ((*it)[0], 4);
        EXPECT_EQ((*it)[1], 5);
        EXPECT_EQ((*it)[2], 6);
        ++it;
        EXPECT_EQ((*it)[0], 7);
        EXPECT_EQ((*it)[1], 8);
        EXPECT_EQ(range.size(), 5);
    }

    {
        range.remove_range(0, 3);
        auto x = range.get_range(0, 2);
        auto it = x.begin();
        EXPECT_EQ((*it)[0], 7);
        EXPECT_EQ((*it)[1], 8);
        EXPECT_EQ(range.size(), 2);
    }
}

TEST(OrderCache, Remove3)
{

    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8};
    Ranges range;
    auto p1 = std::span<int>(v.begin(), v.end());
    range.push_back(p1);
    range.remove_range(2, 3);
    EXPECT_EQ(range.size(), 5);
    auto x = range.get_range(0, 5);
    auto it = x.begin();
    EXPECT_EQ((*it)[0], 1);
    EXPECT_EQ((*it)[1], 2);
    ++it;
    EXPECT_EQ((*it)[0], 6);
    EXPECT_EQ((*it)[1], 7);
    EXPECT_EQ((*it)[2], 8);
}


TEST(OrderCache, Remover2)
{
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8};
    Ranges range;

    auto p1 = std::span<int>(v.begin(), 3);
    auto p2 = std::span<int>(v.begin() + 3, 3);
    auto p3 = std::span<int>(v.begin() + 6, v.end());

    range.push_back(p1);
    range.push_back(p2);
    range.push_back(p3);

    {
        range.remove_range(1, 1);
        auto x = range.get_range(0, 7);
        auto it = x.begin();
        EXPECT_EQ((*it)[0], 1);
        ++it;
        EXPECT_EQ((*it)[0], 3);
        ++it;
        EXPECT_EQ((*it)[0], 4);
        EXPECT_EQ((*it)[1], 5);
        EXPECT_EQ((*it)[2], 6);
        ++it;
        EXPECT_EQ((*it)[0], 7);
        EXPECT_EQ((*it)[1], 8);
        EXPECT_EQ(range.size(), 7);
    }
    {
        range.remove_range(1, 1);
        auto x = range.get_range(0, 2);
        auto it = x.begin();
        EXPECT_EQ((*it)[0], 1);
        ++it;
        EXPECT_EQ((*it)[0], 4);
    }
}