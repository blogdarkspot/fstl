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
		node_ptr tmp = *root;
		node_ptr pos = tmp;
		while(!tmp->_M_nil)
		{
			if(Compare(value, tmp->_M_value.first))
			{
				pos = tmp;
				tmp = tmp->_M_left;
			}
			if(Compare(tmp->_M_value.first, value))
			{
				pos = tmp;
				tmp = tmp->_M_right;
			}
			else
			{
				//value já existe
				return end();
			}
		}

		node_ptr new_node = node_type::allocate();
		//root
		if(pos->_M_nil)
		{
		}
		else
		{
		}
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
		return iterator(root);		
	}
	
	iterator end(node_ptr root)
	{
		return iteator(root->_M_parent);
	}	
};
#endif
