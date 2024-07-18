#ifndef __BINARY_TREE_COMMON__
#define __BINARY_TREE_COMMON__

template<typename T>
struct tree_traits{};

template<typename ValueT, typename T>
struct node_traits{};

template<typename NodeT>
class tree_rotation_policy {
protected:
	static NodeT* left(NodeT* node)
	{
		
		NodeT* right = node->_M_right;
		node->_M_right = right->_M_left;
		if(!node->_M_right->_M_nil)
		{
			node->_M_right->_M_parent = node;
		}
		right->_M_parent = node->_M_parent;
		if(!right->_M_parent->_M_nil)
		{
			if(right->_M_parent->_M_right == node)
			{
				right->_M_parent->_M_right = right;
			}
			else
			{
				right->_M_parent->_M_left = right;
			}
		}
		right->_M_left = node;
		node->_M_parent = right;
			
	}

	static NodeT* right(NodeT** root, NodeT* node)
	{
		NodeT* left = node->_M_left;
		node->_M_left = left->_M_right;
		if(!node->_M_left->_M_nil)
		{
			node->_M_left->_M_parent = node;
		}
		left->_M_parent = node->_M_parent;
		if(!left->_M_parent->_M_nil)
		{
			if(left->_M_parent->_M_right == node)
			{
				left->_M_parent->_M_right = left;
			}
			else
			{
				
				left->_M_parent->_M_left = left;
			}
		}
		left->_M_right = node;
		node->_M_parent = left;
				
	}
};

template<typename Node>
struct tree_iterator
{
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = typename Node::type_value;
	using pointer = value_type*;
	using reference = value_type&;
	using iterator_type = tree_iterator<Node>;


	tree_iterator(pointer source) : _M_data(source) {}

	reference operator*()
	{
		return *_M_data;
	}	
	
	iterator_type& operator++()
	{
		if(_M_data->_M_right->_M_nil)
		{
			auto parent = _M_data->_M_parent;
			while(!parent->_M_nil && 
				parent->_M_left != _M_data)
			{
				_M_data = parent;
				parent = _M_data->_M_parent;	
		   	} 
			_M_data = parent;
		}
		else
		{
			_M_data = tree_min(_M_data->_M_right);
		}
		return *this;
	}

	iterator_type& operator--()
	{
		if(_M_data->_M_left->_M_nil)
		{
			auto parent = _M_data->_M_parent;
			while(!parent->_M_nil &&
				parent->_M_right != _M_data)
			{
				_M_data = parent;
				parent = _M_data->_M_parent;
			}
			_M_data = parent;
		}
		else
		{
			_M_data = tree_max(_M_data->_M_left);
		}
		return *this;
	}

	pointer _M_data;	
};
#endif
