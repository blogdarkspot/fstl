#ifndef __BINARY_TREE__
#define __BINARY_TREE__

#include <utility>
#include <memory>
#include "rb_tree_policy.hpp"

template<typename Key, typename Value, 
			typename Compare, typename Allocator, 
			template<class, class, class, class> class BSTreePolicy>
class BSTree : public BSTreePolicy<Key, Value, Compare, Allocator> {

public:
	using BSTreeType = typename BSTreePolicy<Key, Value, Compare, Allocator>;
	using NodeType = typename BSTreeType::NodeType;
	using iterator = typename BSTreeType::iterator;
	
	iterator insert(std::pair<Key, Value>&& value) {
		return BSTreeType::insert(&_M_root, std::move(value));
	}

	size_t remove(Key&& value) {
		return BSTreeType::remove(&_M_root, std::move(value));
	}

	iterator find(Key&& value) {
		return BSTreeType::find(&_M_root, std::move(value));
	}

	iterator begin() {
		return BSTreeType::begin(_M_root);
	}

	iterator end() {
		return BSTreeType::end(_M_root);
	}

private:
	NodeType* _M_root;
};

template<typename Key, 
			typename Value, 
			typename Compare = std::less<Key>,
			typename Allocator = std::allocator<std::pair<Key, Value>>>
class Map {
	using RedBlackTree = BSTree<Key, Value, Compare, Allocator, RBTreePolicy>;

public:

private:
	RedBlackTree _M_tree;
};
#endif