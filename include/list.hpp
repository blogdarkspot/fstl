#ifndef __LINKEDLIST__
#define __LINKEDLIST__

#include<memory>
#include<utility>
#include<atomic>
#include<iterator>

namespace lf {

	template<typename Value>
	struct Node {

		using Nodeptr = std::shared_ptr<Node<Value>>;

		Node() = default;

		Node(Nodeptr __parent, Nodeptr __next, Value&& __data) : _M_parent(__parent),
			_M_next(__next),
			_M_data(std::move(__data))
		{
		}

		Node(Node&& lhs) :_M_parent(lhs._M_parent), _M_next(lhs._M_next), _M_data(lhs._M_data)
		{
			lhs._M_parent = nullptr;
			lhs._M_next = nullptr;
			lhs._M_data = Value();
		}

		Nodeptr _M_parent = nullptr;
		Nodeptr _M_next = nullptr;
		Value _M_data = Value();
	};

	template<typename Value>
	struct ListIterator {
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = Value;
		using NodeType = Node<Value>;
		using NodePtr = std::shared_ptr<NodeType>;

		ListIterator(NodePtr value) : _M_node(value) {}

		Value operator*() const
		{
			return _M_node->_M_data;
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

		NodePtr _unwarpped() {
			return _M_node;
		}

	private:
		NodePtr _M_node;
	};

	template<typename Value, typename Allocator = std::allocator<Value>>
	class List {
	public:
		using NodeType = Node<Value>;
		using Iterator = ListIterator<Value>;
		using value_type = Value;
		using allocator_type = Allocator;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using refence = value_type&;
		using const_reference = const value_type&;
		using NodePtr = std::shared_ptr<NodeType>;



		List()
		{
			_M_root = std::make_shared<NodeType>();
			_M_end = std::make_shared<NodeType>();
			_M_root->_M_next = _M_end;
			_M_end->_M_parent = _M_root;
		}

		void push_back(Value v)
		{
			auto node = std::make_shared<NodeType>(_M_end->_M_parent, _M_end, std::move(v));
			_M_end->_M_parent->_M_next = node;
			_M_end->_M_parent = node;
			++_M_count;
		}

		std::size_t size() const
		{
			return _M_count;
		}

		Iterator remove(Iterator& it)
		{
			auto node = it._unwarpped();
			auto parent = node->_M_parent;
			auto next = node->_M_next;
			parent->_M_next = next;
			next->_M_parent = parent;
			auto ret = Iterator(next);
			--_M_count;
			return ret;
		}

		void remove(const Value& v)
		{
			auto it = begin();
			auto last = end();
			for (; it != last; ++it)
			{
				if (*it == v)
				{
					remove(it);
					break;
				}
			}
		}

		Iterator begin()
		{
			return Iterator(_M_root->_M_next);
		}

		Iterator end()
		{
			return Iterator(_M_end);
		}

	private:
		inline void next(NodePtr ptr)
		{
			if (ptr == _M_end) return;
			ptr = ptr->_M_next;
		}
		NodePtr _M_root;
		NodePtr _M_end;
		std::size_t _M_count = 0x00;
	};
}
#endif