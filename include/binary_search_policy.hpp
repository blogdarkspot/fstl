#ifndef __BINARY_SEARCH_POLICY__
#define __BINARY_SEARCH_POLICY__

#include<memory>
#include<utility>

#include "common_binary_tree.hpp"


template<typename Value>
struct bs_node
{
	using value_type = Value;
	using node_type = bs_node<Value>;
	using pointer = node_type *;

	pointer _M_parent;
	pointer _M_left;
	pointer _M_right;
	value_type _M_value;

	
	 
};

template<typename ValueT>
struct node_traits<ValueT, bs_node<ValueT>> {
	using value_type = typename bs_node<ValueT>::value_type;
	using node_type  = typename bs_node<ValueT>::node_type;
	using pointer    = typename bs_node<ValueT>::node_type::pointer;	

	template<typename Allocator>
		static pointer allocate_node(Allocator& allocator)
		{
			return  std::allocator_traits<Allocator>::allocate(allocator, 1);
		}

	template<typename Allocator>
		static void free_node(Allocator& allocator, pointer ptr)
		{
			std::allocator_traits<Allocator>::destroy(allocator, ptr);
			std::allocator_traits<Allocator>::deallocate(allocator,ptr, 1);
		}
};


template<typename Key, typename Value, 
			typename Compare,
			typename Allocator>
class BSeachTreePolicy
{
	public:
	using value_type = std::pair<Key, Value>;
	using node_type = bs_node<value_type>;
	using node_ptr = typename node_type::pointer;
	using iterator = tree_iterator<bs_node<value_type>>;	

	iterator insert(node_ptr* root, value_type&& value)
	{
		return iterator(*root);
	}
	
	size_t remove(node_ptr* root, Key&& key)
	{
		return 0;
	}

	iterator find(node_ptr* root, Key&& key)
	{
	}
	
	iterator begin(node_ptr root)
	{
		
	}
	
	iterator end(node_ptr root)
	{
	}	
};
#endif
