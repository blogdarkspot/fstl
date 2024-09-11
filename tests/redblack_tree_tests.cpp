#include <gtest/gtest.h>
#include "rb_tree.hpp"
#include <string>


TEST(RbTree, RotationLeft)
{
    using node_type = RedBlackTree<int, std::string>::node_type;
    auto tree = RedBlackTree<int, std::string>();

    auto root = std::make_shared<node_type>();
    auto left = std::make_shared<node_type>();
    auto right = std::make_shared<node_type>();
    left->m_parent = root;
    right->m_parent = root;
    root->m_parent = tree.m_nil;
    left->m_right = tree.m_nil;
    left->m_left = tree.m_nil;
    right->m_right = tree.m_nil;
    right->m_left = tree.m_nil;
    root->m_left = left;
    root->m_right = right;
    tree.m_root = root;

    EXPECT_EQ(tree.m_root, root);
    EXPECT_EQ(tree.m_root->m_left, left);
    EXPECT_EQ(tree.m_root->m_right, right);
    EXPECT_EQ(tree.m_root->m_left->m_parent, tree.m_root);
    EXPECT_EQ(tree.m_root->m_right->m_parent, tree.m_root);
    EXPECT_EQ(tree.m_root->m_parent, tree.m_nil);

    tree.rotate_left(tree.m_root);
    //validate
    tree.rotate_right(tree.m_root);
    EXPECT_EQ(tree.m_root, root);
    EXPECT_EQ(tree.m_root->m_left, left);
    EXPECT_EQ(tree.m_root->m_right, right);
    EXPECT_EQ(tree.m_root->m_left->m_parent, tree.m_root);
    EXPECT_EQ(tree.m_root->m_right->m_parent, tree.m_root);
    EXPECT_EQ(tree.m_root->m_parent, tree.m_nil);

}

TEST(RbTree, TestInsertAndFixup)
{
    using node_type = RedBlackTree<int, std::string>::node_type;
    auto tree = RedBlackTree<int, int>();
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

TEST(RbTree, TestInsertAndFixup2)
{
    using node_type = RedBlackTree<int, std::string>::node_type;
    auto tree = RedBlackTree<int, int>();
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

TEST(RbTree, TestDeleteAndFixup)
{
    using node_type = RedBlackTree<int, std::string>::node_type;
    auto tree = RedBlackTree<int, int>();
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

    auto result = 1;

    while (begin != end)
    {
        EXPECT_EQ(begin->first, result);
        EXPECT_EQ(begin->second, result);
        ++result;
        ++begin;
    }
    EXPECT_EQ(12, result);

    
    while (--result)
    {
        tree.erase(result); 
    }
    begin = tree.begin();
    EXPECT_EQ(begin, end);
}

TEST(RbTree, TestDeleteAndFixup2)
{
    using node_type = RedBlackTree<int, std::string>::node_type;
    auto tree = RedBlackTree<int, int>();
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
