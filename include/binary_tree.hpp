#ifndef __BINARY_TREE__
#define __BINARY_TREE__
#include <utility>
#include <memory>
#include <set>



template<typename Value, typename Allocator = std::allocator<Value>>
struct node
{
	using value_type = Value;
	using pointer = value_type*;
	using reference = value_type&;
	using node_value = node<Value, Allocator>;
	using node_ptr = node_value*;
	using allocator = Allocator;

	using node_allocator = typename std::allocator_traits<allocator>::template rebind_alloc<node_value>;
	using node_allocator_traits = std::allocator_traits<node_allocator>;

	enum class Color : std::uint8_t {
		RED,
		BLACK
	};

	node() = default;

	Color _M_color = Color::RED;
	node_ptr _M_left = nullptr;
	node_ptr  _M_right = nullptr;
	node_ptr _M_parent = nullptr;
	Value _M_value;

	static node_ptr alloc_node_head() {
		node_ptr ret = new node_value();
		ret->_M_left = ret;
		ret->_M_right = ret;
		ret->_M_parent = ret;
		ret->_M_color = Color::BLACK;
		return ret;
	}

	static node_ptr alloc_node(node_allocator& alloc) {
		node_ptr ret = alloc.allocate(1);
		new (ret) node<Value, Allocator>();
		return ret;
	}

	static node_ptr alloc_head(node_allocator& alloc, node_ptr nil) {
		node_ptr ret = alloc.allocate(1);
		new (ret) node<Value, Allocator>();
		ret->_M_left = nil;
		ret->_M_right = nil;
		ret->_M_parent = nil;
		return ret;
	}

	static void deallocate_node(node_allocator& alloc, node_ptr node) {
		node_allocator_traits::destroy(alloc, node);
		node_allocator_traits::deallocate(alloc, node, 1);
	}
};

template<typename TreeType>
struct binary_tree_iterator {

	using tree_value = TreeType;
	using node_value = typename tree_value::node_value;
	using node_ptr = node_value*;
	using tree_ptr = tree_value*;

	using value_type = typename node_value::value_type;
	using pointer = value_type*;
	using reference = value_type&;
	using difference_type = std::ptrdiff_t;
	using iterator_category = std::bidirectional_iterator_tag;
	

	using iterator = binary_tree_iterator<tree_value>;

	binary_tree_iterator(node_ptr node_ptr, tree_ptr tree) : _M_ptr(node_ptr), _M_tree(tree) {
	}

	iterator& operator--() {
		auto* nil = _M_tree->end()._M_ptr;
		if (_M_ptr->_M_left == nil)
		{
			auto* parent = _M_ptr->_M_parent;
			while (parent != nil && parent->_M_left == _M_ptr)
			{
				_M_ptr = parent;
				parent = _M_ptr->_M_parent;
			}
		}
		else
		{
			_M_ptr = _M_tree->max(_M_ptr->_M_left);
		}
		return *this;
	}

	iterator operator--(int) {
		auto ret = *this;
		--*this;
		return ret;
	}

	iterator& operator++() {
		auto* nil = _M_tree->end()._M_ptr;
		if (_M_ptr->_M_right == nil)
		{
			node_ptr parent = _M_ptr->_M_parent;
			while (parent != nil && parent->_M_right == _M_ptr)
			{
				_M_ptr = parent;
				parent = _M_ptr->_M_parent;
			};
			_M_ptr = parent;
		}
		else
		{
			_M_ptr = _M_tree->min(_M_ptr->_M_right);
		}
		return *this;
	}

	iterator& operator=(const iterator& rhs) {
		_M_ptr = rhs._M_ptr;
		_M_tree = rhs._M_tree;
		return *this;
	}

	iterator operator++(int) {
		auto ret = *this;
		++*this;
		return ret;
	}

	reference operator*() {
		return _M_ptr->_M_value;
	}

	bool operator==(const iterator& rhs) const {
		return _M_ptr == rhs._M_ptr;
	}

	bool operator!=(const iterator& rhs) {
		return _M_ptr != rhs._M_ptr;
	}

	node_ptr _M_ptr;
	tree_ptr _M_tree;
};



template<typename Key, typename Value, typename Allocator = std::allocator<std::pair<Key, Value>>>
struct rb_tree 
{
	using value_type = std::pair<Key, Value>;
	using node_value = node<value_type>;
	using node_ptr = node_value*;
	using alloc_node = typename std::allocator_traits<Allocator>::template rebind_alloc<node_value>;

	using iterator = binary_tree_iterator<rb_tree<Key, Value>>;

	rb_tree() {
		_M_end = node_value::alloc_node_head();
		_M_root = _M_end;
	}

	void insert(const value_type& value) {
		auto tmp = _M_root;
		auto position = tmp;
		node_ptr new_node = nullptr;

		while (tmp != _M_end)
		{
			if (value.first <= tmp->_M_value.first)
			{
				if (tmp->_M_left == _M_end)
				{
					position = tmp;
				}
				tmp = tmp->_M_left;
			}
			else
			{
				if (tmp->_M_right == _M_end)
				{
					position = tmp;
				}
				tmp = tmp->_M_right;
			}
		}

		if (position == _M_end)
		{
			new_node = node_value::alloc_head(_M_alloc_node, _M_end);
			new_node->_M_value = value;
			_M_root = new_node;
		}
		else
		{

			new_node = node_value::alloc_head(_M_alloc_node, _M_end);
			new_node->_M_value = value;
			new_node->_M_parent = position;

			if (position->_M_value.first < value.first)
			{
				position->_M_right = new_node;
			}
			else
			{
				position->_M_left = new_node;
			}
		}
		rb_insert_fixup(new_node);
	}

	void rb_insert_fixup(node_ptr n)
	{
		while (n->_M_parent->_M_color == node_value::Color::RED)
		{
			if (n->_M_parent == n->_M_parent->_M_parent->_M_left)
			{
				auto y = n->_M_parent->_M_parent->_M_right;
				if (y->_M_color == node_value::Color::RED)
				{
					n->_M_parent->_M_color = node_value::Color::BLACK;
					y->_M_color = node_value::Color::BLACK;
					n->_M_parent->_M_parent->_M_color = node_value::Color::RED;
					n = n->_M_parent->_M_parent;
				}
				else
				{
					node_ptr tmp = n;
					if (tmp->_M_parent->_M_right == n)
					{
						tmp = rotation_left(tmp->_M_parent);
					}
					tmp->_M_parent->_M_color = node_value::Color::BLACK;
					tmp->_M_parent->_M_parent->_M_color  = node_value::Color::RED;
					tmp = rotation_right(tmp->_M_parent->_M_parent);
				}
			}
			else
			{
				auto y = n->_M_parent->_M_parent->_M_left;
				if (y->_M_color == node_value::Color::RED)
				{
					y->_M_color = node_value::Color::BLACK;
					n->_M_parent->_M_color = node_value::Color::BLACK;
					n->_M_parent->_M_parent->_M_color = node_value::Color::RED;
					n = n->_M_parent->_M_parent;
				}
				else
				{
					node_ptr tmp = n;
					if (tmp->_M_parent->_M_left == n)
					{
						tmp = rotation_right(tmp->_M_parent);
					}
					tmp->_M_parent->_M_color = node_value::Color::BLACK;
					tmp->_M_parent->_M_parent->_M_color = node_value::Color::RED;
					tmp = rotation_left(tmp->_M_parent->_M_parent);
				}
			}
		}
		_M_root->_M_color = node_value::Color::BLACK;
	}

	void transplant(node_ptr x, node_ptr y)
	{
		if (y->_M_parent == _M_end)
		{
			_M_root = y;
		}
		else if (y->_M_parent->_M_left == x)
		{
			y->_M_parent->_M_left = y;
		}
		else
		{
			y->_M_parent->_M_right = y;
		}
	}

	node_ptr find(Key _key)
	{
		auto tmp = _M_root;
		while (tmp != _M_end && tmp->_M_value.first != _key)
		{
			if (_key <= tmp->_M_value.first)
			{
				tmp = tmp->_M_left;
			}
			else
			{
				tmp = tmp->_M_right;
			}
		}
		return tmp;
	}

	void remove(Key _key) {

		node_ptr node_remove = find(_key);
		if (node_remove != _M_end)
		{
			auto originalColor = node_remove->_M_color;
			node_ptr left = node_remove->_M_left;
			node_ptr right = node_remove->_M_right;
			node_ptr fix_node = _M_end;

			if (right != left)
			{
				if (right == _M_end)
				{
					fix_node = left;
					if (left == _M_end)
					{
						if (node_remove->_M_parent->_M_left == node_remove)
						{
							node_remove->_M_parent->_M_left = _M_end;
						}
						else
						{
							node_remove->_M_parent->_M_right = _M_end;
						}
					}
					else
					{
						transplant(node_remove, left);
					}
				}
				else if (left == _M_end)
				{
					fix_node = right;
					transplant(node_remove, right);
				}
				else
				{
					node_ptr min_right = min(node_remove->_M_right);
					originalColor = min_right->_M_color;
					min_right->_M_color = node_remove->_M_color;
					fix_node = min_right->_M_right;

					if (node_remove->_M_right == min_right)
					{
						transplant(node_remove, min_right);
					}
					else
					{
						transplant(min_right, min_right->_M_right);
						min_right->_M_right = node_remove->_M_right;
						min_right->_M_right->_M_parent = min_right;
					}

					min_right->_M_left = node_remove->_M_left;
					min_right->_M_left->_M_parent = min_right;
				}
			}

			if (originalColor == node_value::Color::BLACK)
			{
				fixup_delete(fix_node);
			}
		}
	}

	void fixup_delete(node_ptr node)
	{
		//se o preto foi removido significa que ficou desbalanceado.
		//ent�o temos que propagar os n�s pretos um n�vel acima, isso ir� fazer 1 n� preto se tornar 2
		// isso naturalmente ir� balancear e ajustar o n� preto que foi removido. 
		while (node != _M_root && node->_M_color == node_value::Color::BLACK)
		{
			//Ok: Agora precisamos saber se o n� que foi removido � esquerdo ou direto
			if (node->_M_parent->_M_left == node)
			{
				node_ptr sibling = node->_M_parent->_M_right;
				if (sibling->_M_color == node_type::Color::RED)
				{
					sibling->_M_color = node_type::Color::BLACK;
					node->_M_parent->_M_color = node_type::Color::RED;
					rotation_left(node->_M_parent);
					sibling = node->_M_parent->_M_right;
				}

				if (sibling->_M_left->_M_color == node_type::Color::BLACK &&
					sibling->_M_right->_M_color == node_type::Color::BLACK)
				{
					sibling->_M_color = node_type::Color::RED;
					node = node->_M_parent;
				}
				else
				{
					if (sibling->_M_right->_M_color == node_type::Color::BLACK)
					{
						sibling->_M_left->_M_color = node_type::Color::BLACK;
						sibling->_M_color = node_type::Color::RED;
						rotation_right(sibling);
						sibling = node->_M_parent->_M_right;
					}
					sibling->_M_color = node->_M_parent->_M_color;
					node->_M_parent->_M_color = node_type::Color::BLACK;
					sibling->_M_right->_M_color = node_type::Color::BLACK;
					rotation_left(node->_M_parent);
					node = _M_root;
				}
			}
			else
			{
				node_ptr sibling = node->_M_parent->_M_left;
				if (sibling->_M_color == node_value::Color::RED)
				{
					node->_M_parent->_M_color = node_value::Color::RED;
					sibling->_M_color = node_value::Color::BLACK;
					rotation_right(node->_M_parent);
					sibling = node->_M_parent->_M_left;
				}

				if (sibling->_M_left->_M_color == node_value::Color::BLACK &&
					sibling->_M_right->_M_color == node_value::Color::BLACK)
				{
					sibling->_M_color = node_value::Color::RED;
					node = node->_M_parent;
				}
				else
				{
					if (sibling->_M_left->_M_color == node_value::Color::RED)
					{
						sibling->_M_color = node_value::Color::RED;
						sibling->_M_right->_M_color = node_value::Color::BLACK;
						rotation_left(sibling);
						sibling = node->_M_parent->_M_left;
					}

					sibling->_M_color = node->_M_parent->_M_color;
					node->_M_parent->_M_color = node_value::Color::BLACK;
					sibling->_M_left->_M_color = node_value::Color::BLACK;
					rotation_right(node->_M_parent);
					node = _M_root;
				}
			}
		}
		node->_M_color = node_value::Color::BLACK;
	}

	void delete_all_nodes(node_ptr value)
	{
		if (value != nullptr && value != _M_end)
		{
			delete_all_nodes(value->_M_left);
			delete_all_nodes(value->_M_right);
			node_value::deallocate_node(_M_alloc_node, value);
			value = nullptr;
		}
	}

	iterator begin()
	{
		auto* first = min(_M_root);
		return iterator(first, this);
	}

	iterator end()
	{
		return iterator(_M_end, this);
	}

	~rb_tree() {
		delete_all_nodes(_M_root);
		delete _M_end;
	}

	node_ptr min(node_ptr value)
	{
		while (value->_M_left != _M_end)
		{
			value = value->_M_left;
		}
		return value;
	}

	node_ptr max(node_ptr value)
	{
		while (value->_M_right != _M_end)
		{
			value = value->_M_right;
		}
		return value;
	}

	node_ptr rotation_left(node_ptr value)
	{
		node_ptr right = value->_M_right;
		value->_M_right = right->_M_left;
		if (right->_M_left != _M_end)
		{
			right->_M_left->_M_parent = value;
		}

		right->_M_parent = value->_M_parent;
		if (right->_M_parent == _M_end)
		{
			_M_root = right;
		}
		else if (value == value->_M_parent->_M_left)
		{
			value->_M_parent->_M_left = right;
		}
		else
		{
			value->_M_parent->_M_right = right;
		}
		right->_M_left = value;
		value->_M_parent = right;
		return right;
	}

	node_ptr rotation_right(node_ptr value)
	{
		node_ptr left = value->_M_left;
		value->_M_left = left->_M_right;
		if (value->_M_left != _M_end)
		{
			value->_M_left->_M_parent = value;
		}
		left->_M_parent = value->_M_parent;
		if (left->_M_parent == _M_end)
		{
			_M_root = left;
		}
		if (left->_M_parent->_M_right == value)
		{
			left->_M_parent->_M_right = left;
		}
		else
		{
			left->_M_parent->_M_left = left;
		}
		left->_M_right = value;
		value->_M_parent = left;
		return left;
	}

	void create_node(node_ptr* __ptr) {
		(*__ptr) = new node_value();
		(*__ptr)->_M_left = _M_end;
		(*__ptr)->_M_right = _M_end;
		(*__ptr)->_M_parent = _M_end;
	}

	node_ptr _M_end;
	node_ptr _M_root;
	alloc_node _M_alloc_node;
};
#endif