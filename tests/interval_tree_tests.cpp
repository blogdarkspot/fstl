#include "interval_tree.hpp"
#include <gtest/gtest.h>

TEST(IntervalTree, Insert)
{
    OSIntervalTree<int, int> container;
    container.insert(10, 10, 10);
    container.insert(9, 9, 9);
    container.insert(8, 8, 8);
    container.insert(4, 4, 4);
    container.insert(3, 3, 3);
    container.insert(5, 5, 5);
    container.insert(6, 6, 6);
    container.insert(7, 7, 7);
    auto it = container.begin();
    EXPECT_EQ(3, it->first);
    EXPECT_EQ(3, it->second);
    EXPECT_EQ(3, it.m_data->m_size);
    ++it;
    EXPECT_EQ(4, it->first);
    EXPECT_EQ(4, it->second);
    EXPECT_EQ(12, it.m_data->m_size);
    ++it;
    EXPECT_EQ(5, it->first);
    EXPECT_EQ(5, it->second);
    EXPECT_EQ(5, it.m_data->m_size);
    ++it;
    EXPECT_EQ(6, it->first);
    EXPECT_EQ(6, it->second);
    EXPECT_EQ(52, it.m_data->m_size);
    ++it;
    EXPECT_EQ(7, it->first);
    EXPECT_EQ(7, it->second);
    EXPECT_EQ(7, it.m_data->m_size);
    ++it;
    EXPECT_EQ(8, it->first);
    EXPECT_EQ(8, it->second);
    EXPECT_EQ(15, it.m_data->m_size);
    ++it;
    EXPECT_EQ(9, it->first);
    EXPECT_EQ(9, it->second);
    EXPECT_EQ(34, it.m_data->m_size);
    ++it;
    EXPECT_EQ(10, it->first);
    EXPECT_EQ(10, it->second);
    EXPECT_EQ(10, it.m_data->m_size);
}

TEST(IntervalTree, Remove)
{
    OSIntervalTree<int, int> container;
    container.insert(10, 10, 10);
    container.insert(9, 9, 9);
    container.insert(8, 8, 8);
    container.insert(4, 4, 4);
    container.insert(3, 3, 3);
    container.insert(5, 5, 5);
    container.insert(6, 6, 6);
    container.insert(7, 7, 7);
    container.erase(3);
    container.erase(10);
    container.erase(6);
    auto it = container.begin();
    EXPECT_EQ(4, it->first);
    EXPECT_EQ(4, it->second);
    EXPECT_EQ(9, it.m_data->m_size);
    ++it;
    EXPECT_EQ(5, it->first);
    EXPECT_EQ(5, it->second);
    EXPECT_EQ(5, it.m_data->m_size);
    ++it;
    EXPECT_EQ(7, it->first);
    EXPECT_EQ(7, it->second);
    EXPECT_EQ(33, it.m_data->m_size);
    ++it;

    EXPECT_EQ(8, it->first);
    EXPECT_EQ(8, it->second);
    EXPECT_EQ(17, it.m_data->m_size);

    ++it;
    EXPECT_EQ(9, it->first);
    EXPECT_EQ(9, it->second);
    EXPECT_EQ(9, it.m_data->m_size);
}

TEST(IntervalTree, OSSearch)
{
    OSIntervalTree<int, int> container;
    container.insert(1, 1, 1);
    container.insert(2, 2, 2);
    container.insert(3, 3, 3);
    container.insert(4, 4, 4);
    container.insert(5, 5, 5);
    container.insert(6, 6, 6);
    container.insert(7, 7, 7);
    container.insert(8, 8, 8);

    {
        auto result = container.os_search(1);
        EXPECT_EQ(1, result.first->first);
        EXPECT_EQ(1, result.first->second);
        EXPECT_EQ(1, result.second);
    }
    {
        auto result = container.os_search(2);
        EXPECT_EQ(2, result.first->first);
        EXPECT_EQ(2, result.first->second);
        EXPECT_EQ(1, result.second);
    }
    {
        auto result = container.os_search(3);
        EXPECT_EQ(2, result.first->first);
        EXPECT_EQ(2, result.first->second);
        EXPECT_EQ(2, result.second);
    }
}
