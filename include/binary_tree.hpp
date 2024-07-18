#ifndef __BINARY_TREE__
#define __BINARY_TREE__

#include <utility>
#include <memory>

template<typename Key, typename Value, 
			typename Compare, typename Allocator, 
			template<class, class, class, class> class BinaryTreePolicy>
class BinaryTree : public BinaryTreePolicy<Key, Value, Compare, Allocator> {

public:
	using BinaryTreeType = BinaryTreePolicy<Key, Value, Compare, Allocator>;
	using node_type = typename BinaryTreeType::node_type;
	using iterator = typename BinaryTreeType::iterator;
	
	iterator insert(std::pair<Key, Value>&& value) {
		return BinaryTreeType::insert(&_M_root, std::move(value));
	}

	size_t remove(Key&& value) {
		return BinaryTreeType::remove(&_M_root, std::move(value));
	}

	iterator find(Key&& value) {
		return BinaryTreeType::find(&_M_root, std::move(value));
	}

	iterator begin() {
		return BinaryTreeType::begin(_M_root);
	}

	iterator end() {
		return BinaryTreeType::end(_M_root);
	}

private:
	node_type* _M_root;
};
#endif
