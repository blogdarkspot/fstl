#ifndef __RB_TREE_POLICY__
#define __RB_TREE_POLICY__

#include<utility>
#include"common_binary_tree.hpp"

class NullType
{
	enum
	{
		enabled = false
	};
};

template <typename T, typename Allocator>
struct Node2
{
    using value_type = T;
	using allocator = Allocator;
	using node_type = Node2<value_type, allocator>;
    using properties_type = NullType;

    value_type _M_value;
    properties_type _M_properties;	
	node_type* _M_left;
    node_type* _M_right;
};


struct RBProperties
{
    enum
    {
        enabled = true
    };

	enum 
	{
		RED = true,
		BLACK = false
	} _M_color = RED;
};

struct OrderStatisticsProperties : public RBProperties
{
    size_t _M_size = 0;
};

template <typename T, typename Properties = NullType, 
	typename Allocator = std::allocator<T>>
struct Node
{
	private:
	  using node_type = Node<T, Properties, Allocator>;
  public:
    using value_type = T;
	using allocator = Allocator;
	using alloc_traits = typename std::allocator_traits<allocator>;
    using properties_type = Properties;

	Node() = default;

	Node(value_type &&value) : _M_value(std::move(value))
    {
    }

	const properties_type& get_cproperties() const
	{
        return _M_properties;
	}

	properties_type& get_properties()
    {
        return _M_properties;
	}

    properties_type _M_properties;
    value_type _M_value;
	node_type* _M_left;
    node_type* _M_right;
    node_type* _M_parent;
    bool _M_isNil;
};

template<typename NodeT>
struct NodeTraits
{
	using value_type = NodeT;
	using pointer = value_type*;
	using reference = value_type&;
	using allocator = typename std::allocator_traits<typename NodeT::allocator>::template rebind_alloc<NodeT>;
	using iterator = tree_iterator<NodeT>;

	using properties_type = typename value_type::properties_type;

	template<typename... Params>
	static pointer create(allocator& alloc, Params &&...args)
	{
		using alloc_traits = typename std::allocator_traits<allocator>;
        pointer ret = alloc_traits::allocate(alloc, 1);
        alloc_traits::construct(alloc, ret, std::forward<Params>(args)...);
        return ret;
	}

	static pointer delete_node(allocator& alloc, pointer node)
	{
		using alloc_traits = typename std::allocator_traits<allocator>;
        alloc_traits::destroy(alloc, node);
        alloc_traits::deallocate(alloc, node, 1);
	}
	
	static pointer get_parent(pointer node)
	{
        return node->_M_parent;
	}

	static pointer get_left(pointer node)
	{
        return node->_M_left;
	}

	static pointer get_right(pointer node)
	{
        return node->_M_right;
	}

	/**     P               P
	*       |               |
			x				Y
		   / \             / \
		  Z   Y  ----->   X   B
			 / \		 / \
		    A  B        Z   A  
	*/
	static pointer rotate_left(pointer& node)
	{
        auto r = node->_M_right;
        r->_M_parent = node->_M_parent;
		if (!r->_M_parent->_M_isNil)
		{
            r->_M_parent = r;
		}
        node->_M_parent = r;
		if (r->_M_left)
		{
            node->_M_right = r->_M_left;
            r->_M_left->_M_parent = node;
		}
        r->_M_left = node;
        return r;
	}

	/********************************
	        P               P
	        |               |
			x				Z
		   / \             / \
		  Z   Y  ----->   A   X
		 / \     			 / \
		A  B                B   Y  
	*********************************/
	static pointer rotate_right(pointer& node)
	{
        auto l = node->_M_left;
        auto l->_M_parent = node->_M_parent;
		if (!l->_M_parent->_M_isNil)
		{
            l->_M_parent = l;
		}
        node->_M_parent = l;
		if (!l->_M_right->_M_isNil)
		{
            node->_M_left = l->_M_right;
            node->_M_left->_M_parent = node;
		}
        l->_M_right = node;
        return l;
	}

	static bool less(pointer x, pointer y)
	{
        return x->_M_value < y->_M_value;
	}
};


template<typename Node> class OrderStatistics
{
  public:

	  Node* select(Node* root, size_t rank)
	  {
          auto r = root->_M_left->_M_properties._M_size + 1; 
		  if (r == rank)
              return root;
		  else if (rank < r)
		  {
              return select(root->_M_left, rank);
		  }
		  else
		  {
              return select(root->_M_right, rank - r);
		  }
	  }

	  size_t rank(Node* root, Node* node)
	  {
          size_t ret += node->_M_left->_M_properties._M_size + 1;
		  while (node != root)
		  {
			  if (node->_M_parent->_M_right == node)
			  {
                  ret += node->_M_parent->_M_left->_M_properties._M_size + 1;
			  }
              node = node->_M_parent;
		  } 

          return ret;
	  }
};

template<typename Node,
	template<class> typename Compare = std::less> class Delete
{
	using node_traits = NodeTraits<Node>;
	using value_type = typename node_traits::value_type;

	void erase(value_type** root, value_type* val)
	{
		
		if (val->_M_left->_M_isNil)
		{
            transplat(val, val->_M_right);
			if (val->_M_parent->_M_isNil)
			{
                (*root) = val;
			}
		}
		else if (val->_M_right->_M_isNil)
		{
            transplat(val, val->_M_left);
			if (val->_M_parent->_M_isNil)
			{
                (*root) = val;
			}
		}
		else
		{
		}
	}


	value_type* minimum(value_type* val)
	{
		while (!val->_M_left->_M_properties._M_isNil)
		{
            val = val->_M_left;
		}
        return val;
	}

	void transplat(value_type* x, value_type* y)
	{
		if (x->_M_parent->right == x)
		{
            x->_M_parent->_right = y;
		}
		else
		{
            x->_M_parent->_left = y;
		}
		if (!y->_M_isNil)
		{
            y->_M_parent = x->_M_parent;
		}
	}
};

template<typename Node, 
	template<class> typename Compare = std::less> class InsertRBTree
{
  public:
	using node_traits = NodeTraits<Node>;
	using value_type = typename node_traits::value_type;
	using alloc_node = typename node_traits::allocator;
	using node_iterator = typename node_traits::iterator;
	using value = typename value_type::value_type;
	using compare_func = typename Compare<value>;

	void insert(Node** root, alloc_node& allocator, value&& val)
	{
        auto root_ = *root;
        auto found_ = root_;
        compare_func compare_;

		while (!found_->_M_isNil)
		{
            root_ = found_;
			if (compare_(found_->_M_value, val))
			{
                found_ = found_->_M_left;
			}
            else if (compare_(val, found_->_M_value))
			{
                found_ = found_->_M_right;
			}
			else
			{
                break;
			}
		}

		if (!found_->_M_isNil)
		{

		}
		
		auto new_node = node_traits::create(allocator, std::move(val));

		new_node->_M_parent = root_;
        new_node->_M_right = root_->_M_parent;
        new_node->_M_left = root_->_M_parent;

		if (root_->_M_isNil)
		{
            *root = root_;
		}
		else
		{
			if (compare_(root_->_M_value, new_node->_M_value))
			{
                root_->_M_left = new_node;	
			}
			else
			{
                root_->_M_right = new_node;
			}
		}
	}

	private:

	void fixup_tree(Node** root, Node* new_node)
	{
        new_node->_M_properties._M_color = RBProperties::RED;

		while (new_node->_M_parent->_M_properties.Color == RBProperties::RED)
		{
            auto parent = new_node->_M_parent;
			
			if (parent->_M_parent->_M_right == parent)
			{
                auto uncle = parent->_M_parent->_M_left;

				if (uncle->_M_properties.Color == RBProperties::RED)
				{
                    uncle->_M_properties.Color = RBProperties::BLACK;
                    parent->_M_properties.Color = RBProperties::BLACK;
                    parent->_M_parent->_M_properties.Color = RBProperties::RED;
                    new_node = parent->_M_parent;
				}
				else
				{
					if (new_node == parent->left)
					{
                       new_node = parent;
                       auto ret = node_traits::rotate_right(new_node);
					   if (ret->_M_parent->_M_isNil)
					   {
                           *root = ret;
					   }
					}
                    new_node->_M_parent->_M_properties.Color = RBProperties::BLACK;
                    new_node->_M_parent-> _M_parent->_M_properties.Color = RBProperties::RED;
                    new_node = parent->_M_parent->_M_parent;
                    auto ret = node_traits::rotate_left(new_node);
					if (ret->_M_parent->_M_isNil)
					{
                        *root = ret;
					}
				}
			}
			else
			{
                auto uncle = parent->_M_parent->_M_right;
				if (uncle->_M_properties._M_color == RBProperties::RED)
				{
                    uncle->_M_properties._M_color = RBProperties::BLACK;
                    parent->_M_properties._M_color = RBProperties::BLACK;
                    parent->_M_parent->_M_properties._M_color = RBProperties::RED;
                    new_node = parent->_M_parent;
				}
				else
				{
					if (new_node == parent->_M_right)
					{
						auto ret = node_traits::rotate_left(parent)
						if(ret->_M_parent->_M_isNil) {
                            *root = ret;
						}
                        new_node = parent;
					}

					new_node->_M_parent->_M_properties._M_color = RBProperties::BLACK;
                    new_node->_M_parent->_M_parent->_M_properties._M_color = RBProperties::RED;
                    new_node = new_node->_M_parent->_M_parent;	
					auto ret = node_traits::rotate_right(new_node);
					if (ret->_M_parent->_M_isNil)
					{
                        *root = ret;
					}
				}
			}

		}
        (*root)->_M_properties._M_Color = RBProperties::BLACK;
	}
};

template<typename Key, typename Value, typename Compare, typename Allocator>
class RBTreePolicy {

	struct node
	{
		using value_type = std::pair<Key, Value>;
		using pointer = value_type*;
		using reference = value_type&;
		using node_value = node;
		using node_ptr = node_value*;
		using allocator = Allocator;

		using node_allocator = typename std::allocator_traits<allocator>::template rebind_alloc<node_value>;
		using node_allocator_traits = std::allocator_traits<node_allocator>;

		enum class Color : std::uint8_t {
			RED,
			BLACK
		};

		node() = default;

		bool _M_nil = true;
		Color _M_color = Color::RED;
		node_ptr _M_left = nullptr;
		node_ptr  _M_right = nullptr;
		node_ptr _M_parent = nullptr;
		Value _M_value;

		static node_ptr create_node(node_allocator& alloc, node_ptr nil) {
			node_ptr ret = alloc.allocate(1);
			ret->_M_left = nil;
			ret->_M_right = nil;
			ret->_M_parent = nil;
			return ret;
		}

		static void release_node(node_allocator& alloc, node_ptr node) {
			node_allocator_traits::destroy(alloc, node);
			node_allocator_traits::deallocate(alloc, node, 1);
		}
	};

protected:

	using TreeType = RBTreePolicy<Key, Value, Compare, Allocator>;
	using node_type = node;
	using iterator = tree_iterator<node_type>;

	using ValueType = Value;

	iterator insert(node_type** _M_root, ValueType&& _value) {
		node_type* tmp = *_M_root;
		node_type* position = tmp;
		while (!tmp->_M_nil)
		{
			position = tmp;
			if (_value.first == tmp->_M_value.first)
			{
				break;
			}

			if (Compare(_value.first, tmp->_M_value.first))
			{
				tmp = tmp->_M_left;
			}
			else
			{
				tmp = tmp->_M_right;
			}
		}

		//root
		if (position->_M_nil)
		{

		}
		else
		{

		}
		return iterator(position);
	}

	std::size_t remove(node_type** _M_root, Key _key) {
		return  0;
	}

	iterator begin(node_type* root) {
		return iterator(root);
	}

	iterator end(node_type* root) {
		return iterator(root->_M_parent);
	}

	static node_type* min(node_type* root) {
		auto ret = root;
		while (!ret->_M_left->_M_nil)
		{
			ret = ret->_M_left;
		}
		return ret;
	}

	static node_type* max(node_type* root) {
		auto ret = root;
		while (!ret->_M_right->_M_nil)
		{
			ret = ret->_M_right;
		}
		return ret;
	}

	static node_type* leftRotation(node_type* node) {
		auto right = node->_M_right;
		node->_M_right = right->_M_left;
		if (!right->_M_left->_M_nil)
		{
			right->_M_left->_M_parent = node;
		}
		right->_M_left = node;
		right->_M_parent = node->_M_parent;
		node->_M_parent = right;
		if (!right->_M_parent->_M_nil)
		{
			if (right->_M_parent->_M_right == node)
			{
				right->_M_parent->_M_right = right;
			}
			else
			{
				right->_M_parent->_M_left = right;
			}
		}
		return right;
	}
	//if parent is nil change the root
	static node_type* rightRotation(node_type* node) {
		auto left = node->_M_left;
		node->_M_left = left->_M_right;
		if (!node->_M_left->_M_nil)
		{
			node->_M_left->_M_parent = node;
		}
		node->_M_parent = left;
		left->_M_right = node;
		left->_M_parent = node->_M_parent;
		if (!left->_M_parent->_M_nil)
		{
			if (left->_M_parent->_M_right == node)
			{
				left->_M_parent->_M_right = left;
			}
			else
			{
				left->_M_parent->_M_left = left;
			}
		}
		return left;
	}
};

/*
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
		//então temos que propagar os nós pretos um nível acima, isso irá fazer 1 nó preto se tornar 2
		// isso naturalmente irá balancear e ajustar o nó preto que foi removido. 
		while (node != _M_root && node->_M_color == node_value::Color::BLACK)
		{
			//Ok: Agora precisamos saber se o nó que foi removido é esquerdo ou direto
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
*/

#endif
