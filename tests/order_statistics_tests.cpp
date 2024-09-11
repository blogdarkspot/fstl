#include "order_statistics.hpp"
#include <gtest/gtest.h>


TEST(OrderStatistics, TestInsertAndFindByRank)
{
    using node_type = OrderStatisticRBtree<int, int>::node_type;
    auto tree = OrderStatisticRBtree<int, int>();
    tree.insert(1, 1);
    tree.insert(2, 2);
    tree.insert(3, 3);
    tree.insert(4, 4);
    tree.insert(5, 5);
    tree.insert(6, 6);
    tree.insert(7, 7);
    tree.insert(8, 8);
    tree.insert(9, 9);
    tree.insert(10, 10);
    tree.insert(11, 11);

    for (size_t i = 1; i <= 11; ++i)
    {
        auto it = tree.os_search(i);
        EXPECT_EQ(it->first, i);
    }
}

TEST(OrderStatistics, TestInsertAndFixup2)
{
    using node_type = OrderStatisticRBtree<int, int>::node_type;
    auto tree = OrderStatisticRBtree<int, int>();
    tree.insert(11, 11);
    tree.insert(10, 10);
    tree.insert(9, 9);
    tree.insert(8, 8);
    tree.insert(7, 7);
    tree.insert(6, 6);
    tree.insert(5, 5);
    tree.insert(4, 4);
    tree.insert(3, 3);
    tree.insert(2, 2);
    tree.insert(1, 1);
    auto begin = tree.begin();
    auto end = tree.end();

    auto result = 1;

    while (begin != end)
    {
        EXPECT_EQ(begin->first, result);
        EXPECT_EQ(begin->second, result);
        ++result;
        ++begin;
    }
    EXPECT_EQ(12, result);
}

TEST(OrderStatistics, TestDeleteAndFixup)
{
    using node_type = OrderStatisticRBtree<int, int>::node_type;
    auto tree = OrderStatisticRBtree<int, int>();
    tree.insert(1, 1);
    tree.insert(2, 2);
    tree.insert(3, 3);
    tree.insert(4, 4);
    tree.insert(5, 5);
    tree.insert(6, 6);
    tree.insert(7, 7);
    tree.insert(8, 8);
    tree.insert(9, 9);
    tree.insert(10, 10);
    tree.insert(11, 11);
   
    auto begin = tree.begin();
    auto end = tree.end();



    auto result = 12;
    
    while (--result)
    {
        auto it = tree.os_search(result);
        EXPECT_EQ(it->first, result);
        tree.erase(result); 
    }
    begin = tree.begin();
    EXPECT_EQ(begin, end);
}

TEST(OrderStatistics, TestDeleteAndFixup2)
{
    using node_type = OrderStatisticRBtree<int, int>::node_type;
    auto tree = OrderStatisticRBtree<int, int>();
    tree.insert(11, 11);
    tree.insert(10, 10);
    tree.insert(9, 9);
    tree.insert(8, 8);
    tree.insert(7, 7);
    tree.insert(6, 6);
    tree.insert(5, 5);
    tree.insert(4, 4);
    tree.insert(3, 3);
    tree.insert(2, 2);
    tree.insert(1, 1);
    auto begin = tree.begin();
    auto end = tree.end();

    auto result = 1;

    while (begin != end)
    {
        EXPECT_EQ(begin->first, result);
        EXPECT_EQ(begin->second, result);
        ++result;
        ++begin;
    }
    EXPECT_EQ(12, result);
    for(auto i = 1; i <= result; ++i)
    {
        tree.erase(i); 
    }
    begin = tree.begin();
    EXPECT_EQ(begin, end);
}

#include<vector>

TEST(HackerRank, case1)
{
    /*
    struct command
    {
        int cmd;
        int begin;
        int end;
    };

    size_t n = 0, m = 0;

    std::cin >> n >> m;

    auto tree = OrderStatisticRBtree<std::int64_t, std::int64_t>();
    std::int64_t begin = 0x00;
    std::int64_t end = 0x00;
    size_t tree_size = n;
    while (n--)
    {
        int value = 0;
        std::cin >> value;
        tree.insert(end++, value);
    }

    while(--m)
    {
        int c = 0;
        int init = 0;
        int last = 0;

        std::cin >> c >> init >> last;

        std::vector<OrderStatisticRBtree<std::int64_t, std::int64_t>::iterator> positions;

        while (init <= last)
        {
            auto it = tree.os_search(init);
            positions.emplace_back(it);
            ++init;
        }

        for (int i = positions.size() - 1; i >= 0; --i)
        {

            tree.erase(positions[i]->first);
        }
        
        if (c == 1)
        {
            for (int i = positions.size() - 1; i >= 0; --i)
            {
                tree.insert(--begin, positions[i]->second);
            }
        }
        else
        {
            for (int i = 0; i < positions.size(); ++i)
            {
                tree.insert(++end, positions[i]->second);
            }
        }
    }

    ///////
    auto x = tree.os_search(1);
    auto y = tree.os_search(tree.m_size);

    int diff = y->second - x->second;
    if (diff < 0)
        diff *= -1;
    std::cout << diff << std::endl;

    {
        auto it = tree.begin();
        auto _end = tree.end();
        while (!(it == _end))
        {
            std::cout << it->second << " ";
            ++it;
        }
    } */
    
}