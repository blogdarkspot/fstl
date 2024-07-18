#ifndef __LINKEDLIST__
#define __LINKEDLIST__

#include <memory>
#include <iterator>

template<typename Value>
struct NodeList {
	using value_type  = Value;
	using node_type_ = NodeList<Value>;
	using node_ptr_ = node_type_*;

	NodeList() = default;

	node_ptr_ _M_parent = nullptr;
	node_ptr_ _M_next = nullptr;
	value_type _M_value = Value();

	template<typename Allocator>
	static node_ptr_ create_node(Allocator& allocator) 
	{
		node_ptr_ node = std::allocator_traits<Allocator>::allocate(allocator, 1);
		::new (static_cast<void *>(node)) node_type_();
		return node;
	}

	template<typename Allocator>
	static void free_node(Allocator& allocator, node_ptr_ node)
	{
		node->~node_type_();
		std::allocator_traits<Allocator>::destroy(allocator, node);
	}
};

template<typename Value>
struct ListIterator {
	//iterator traits
	using iterator_category = std::bidirectional_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = Value;
	using pointer = value_type*;
	using reference = value_type&;

	using node_type_ = typename NodeList<Value>::node_type_;
	using node_ptr_ = typename node_type_::node_ptr_;

	ListIterator(node_ptr_ value) : _M_node(value) {}

	Value operator*() const
	{
		return _M_node->_M_value;
	}

	ListIterator& operator++()
	{
		_M_node = _M_node->_M_next;
		return *this;
	}

	ListIterator operator++(int)
	{
		auto tmp = *this;
		++*this;
		return tmp;
	}

	ListIterator operator--()
	{
		_M_node = _M_node->_M_parent;
		return *this;
	}

	ListIterator operator--(int)
	{
		auto tmp = *this;
		--*this;
		return tmp;
	}

	bool operator==(ListIterator& lhs)
	{
		return _M_node == lhs._M_node;
	}

	bool operator!=(ListIterator& lhs)
	{
		return _M_node != lhs._M_node;
	}
	
	node_ptr_  get_pointer()
	{
		return _M_node;
	}
	private:
	node_ptr_ _M_node;
};

template<typename Value, typename Allocator = std::allocator<Value>>
class List {
	public:
		using Iterator = ListIterator<Value>;
		using value_type = Value;
		using allocator_type = Allocator;

		using node_type_ = typename NodeList<Value>::node_type_;
		using node_ptr_ = typename node_type_::node_ptr_; 

		using allocator_node = typename std::allocator_traits<Allocator>::template rebind_alloc<node_type_>;

		List()
		{
			_M_end = node_type_::create_node(_M_allocator);
			_M_end->_M_next = _M_end;
			_M_end->_M_parent = _M_end;
			_M_root = _M_end;
		}

		~List()
		{
			auto tmp = _M_root;
			while(tmp != _M_end)
			{
				auto rm = tmp;
				tmp = tmp->_M_next;
				node_type_::free_node(_M_allocator, rm);	
			}
			node_type_::free_node(_M_allocator, _M_end);
		}

		void push_back(Value v)
		{
			node_ptr_ new_node = node_type_::create_node(_M_allocator);	
			new_node->_M_value = v;
			if(_M_root == _M_end)
			{
				new_node->_M_parent = new_node;
				new_node->_M_next = _M_end;
				_M_end->_M_parent = new_node;
				_M_root = new_node;
			}
			else
			{
				auto last = _M_end->_M_parent;
				last->_M_next = new_node;
				new_node->_M_parent = last;
				new_node->_M_next = _M_end;
				_M_end->_M_parent = new_node;
			}
			++_M_count;
		}

		size_t erase(Iterator& it)
		{
			auto last = end();
			if(it != last)
			{
				node_ptr_ rm = it.get_pointer();					
				if(rm->_M_parent == rm)
				{
					_M_root = rm->_M_next;
					_M_root->_M_parent = _M_root;
				}
				else
				{
					auto parent = rm->_M_parent;
					parent->_M_next = rm->_M_next;
					rm->_M_next->_M_parent = parent;
				}
				node_type_::free_node(_M_allocator, rm);
				--_M_count;
				return 1;
			}	
			return 0;
		}

		Iterator find(const Value& value)
		{
			auto last = end();
			for(auto it = begin(); it != last; ++it)
			{
				if(*it == value)
				{
					return it;
				}
			}
			return last;
		}

		Iterator begin()
		{
			return Iterator(_M_root);
		}

		Iterator end()
		{
			return Iterator(_M_end);
		}

		std::size_t size() const
		{
			return _M_count;
		}
	private:
		node_ptr_ _M_root;
		node_ptr_ _M_end;
		allocator_node _M_allocator;			
		std::size_t _M_count = 0x00;
};
#endif
