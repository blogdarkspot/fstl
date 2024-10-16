#include <gtest/gtest.h>
#include "rb_tree_policy.hpp"
#include "binary_search_policy.hpp"

TEST(RBTreeInsertPolicy, InsertRoot)
{
	using node_type = Node<int, RBProperties>;
	using node_traits = typename NodeTraits<node_type>;
    node_traits::allocator allocator;
    auto root = node_traits::create(allocator);

    root->_M_parent = root;
    root->_M_left = root;
    root->_M_right = root;
    root->_M_isNil = true;

	auto insert_policy = InsertRBTree<node_traits::value_type>();
    insert_policy.insert(&root, allocator, 1);
}

/*

class BSTreeTest : public ::testing::Test {
	protected:
		using tree = BinaryTree<int, int, std::less<int>, std::allocator<std::pair<int, int>>, BSeachTreePolicy>;
		using node_type = tree::node_type;
		using node_ptr = node_type*;
		using value_type = node_type::value_type;

	tree _M_bsTree;
};

class RBTreeTest : public ::testing::Test {

protected:

using tree = BinaryTree<int, int, std::less<int>, std::allocator<std::pair<int, int>>, RBTreePolicy>;
using node_ptr = tree::node_type*;
using node_value = tree::node_type;
using Color = node_value::Color;



template<typename Key, typename Value>
struct ExpectedNodeValues {
	Key key;
	Value value;
	Color color;
	node_ptr parent;
	node_ptr left;
	node_ptr right;
};


	RBTreeTest() {

	}

	template<typename Key, typename Value>
	void assert_node(node_ptr node, ExpectedNodeValues<Key, Value> result)
	{
		ASSERT_EQ(node->_M_left, result.left);
		ASSERT_EQ(node->_M_right, result.right);
		ASSERT_EQ(node->_M_parent, result.parent);
		//ASSERT_EQ(node->_M_value.second, result.value);
		//ASSERT_EQ(node->_M_value.first, result.key);
		ASSERT_EQ(node->_M_color, result.color);
	}
	
	tree _M_tree;
};
*/

/*
TEST_F(RBTreeTest, InsertRoot)
{
	_M_tree.insert({ 0, 1 });

	auto root = _M_tree._M_root;
	auto nil = _M_tree._M_end;
	auto resultRoot = ExpectedNodeValues<int, int>();
	
	resultRoot.color = Color::BLACK;
	resultRoot.left = nil;
	resultRoot.right = nil;
	resultRoot.parent = nil;
	resultRoot.key = 0;
	resultRoot.value = 1;

	assert_node<int, int>(root, resultRoot);

}

TEST_F(RBTreeTest, InsertLeft)
{
	//Root must be black and point left right and parent to nil
	_M_tree.insert({ 0, 1 });
	_M_tree.insert({ -1, 0 });

	auto root = _M_tree._M_root;
	auto left = root->_M_left;
	auto nil = _M_tree._M_end;

	auto resultLeft = ExpectedNodeValues<int, int>();
	auto resultRoot = ExpectedNodeValues<int, int>();

	resultRoot.color = Color::BLACK;
	resultRoot.left = left;
	resultRoot.right = nil;
	resultRoot.parent = nil;
	resultRoot.key = 0;
	resultRoot.value = 1;

	resultLeft.left = nil;
	resultLeft.right = nil;
	resultLeft.parent = root;
	resultLeft.color = Color::RED;
	resultLeft.key = -1;
	resultLeft.value = 0;

	assert_node<int, int>(root, resultRoot);
	assert_node<int, int>(left, resultLeft);
}

TEST_F(RBTreeTest, InsertRight)
{
	//Root must be black and point left right and parent to nil
	_M_tree.insert({ 0, 1 });
	_M_tree.insert({ 1, 2 });

	auto root = _M_tree._M_root;
	auto right = root->_M_right;
	auto nil = _M_tree._M_end;

	auto resultRight = ExpectedNodeValues<int, int>();
	auto resultRoot = ExpectedNodeValues<int, int>();

	resultRoot.color = Color::BLACK;
	resultRoot.left = nil;
	resultRoot.right = right;
	resultRoot.parent = nil;
	resultRoot.key = 0;
	resultRoot.value = 1;

	resultRight.left = nil;
	resultRight.right = nil;
	resultRight.parent = root;
	resultRight.color = Color::RED;
	resultRight.key = 1;
	resultRight.value = 2;

	assert_node<int, int>(root, resultRoot);
	assert_node<int, int>(right, resultRight);
}

TEST_F(RBTreeTest, InsertLeftRotationBalance)
{
	//Root must be black and point left right and parent to nil
	_M_tree.insert({ 0, 1 });
	_M_tree.insert({ 1, 2 });
	_M_tree.insert({ 2, 3 });

	auto root = _M_tree._M_root;
	auto right = root->_M_right;
	auto left = root->_M_left;
	auto nil = _M_tree._M_end;

	auto resultRight = ExpectedNodeValues<int, int>();
	auto resultLeft = ExpectedNodeValues<int, int>();
	auto resultRoot = ExpectedNodeValues<int, int>();

	resultRoot.color = Color::BLACK;
	resultRoot.left = left;
	resultRoot.right = right;
	resultRoot.parent = nil;
	resultRoot.key = 1;
	resultRoot.value = 2;

	resultRight.left = nil;
	resultRight.right = nil;
	resultRight.parent = root;
	resultRight.color = Color::RED;
	resultRight.key = 2;
	resultRight.value = 3;

	resultLeft.left = nil;
	resultLeft.right = nil;
	resultLeft.parent = root;
	resultLeft.color = Color::RED;
	resultLeft.key = 0;
	resultLeft.value = 1;

	assert_node<int, int>(root, resultRoot);
	assert_node<int, int>(right, resultRight);
	assert_node<int, int>(left, resultLeft);
}

TEST_F(RBTreeTest, InsertRightRotationBalance)
{
	//Root must be black and point left right and parent to nil
	_M_tree.insert({ 2, 3 });
	_M_tree.insert({ 1, 2 });
	_M_tree.insert({ 0, 1 });

	auto root = _M_tree._M_root;
	auto right = root->_M_right;
	auto left = root->_M_left;
	auto nil = _M_tree._M_end;

	auto resultRight = ExpectedNodeValues<int, int>();
	auto resultLeft = ExpectedNodeValues<int, int>();
	auto resultRoot = ExpectedNodeValues<int, int>();

	resultRoot.color = Color::BLACK;
	resultRoot.left = left;
	resultRoot.right = right;
	resultRoot.parent = nil;
	resultRoot.key = 1;
	resultRoot.value = 2;

	resultRight.left = nil;
	resultRight.right = nil;
	resultRight.parent = root;
	resultRight.color = Color::RED;
	resultRight.key = 2;
	resultRight.value = 3;

	resultLeft.left = nil;
	resultLeft.right = nil;
	resultLeft.parent = root;
	resultLeft.color = Color::RED;
	resultLeft.key = 0;
	resultLeft.value = 1;

	assert_node<int, int>(root, resultRoot);
	assert_node<int, int>(right, resultRight);
	assert_node<int, int>(left, resultLeft);
}
*/
