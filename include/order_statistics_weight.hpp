#pragma once
#include <assert.h>
#include <list>
#include <memory>
#include <span>

struct Ranges
{
    using ranges = std::list<std::span<int>>;
    Ranges() = default;
    Ranges(ranges &list, std::size_t size) : m_ranges(list), m_size(size)
    {
    }
    ranges get_range(std::size_t index, std::size_t count)
    {
        assert(index >= 0 && index < m_size);
        assert(count < m_size || index == 0 && count == m_size);
        ranges ret;
        auto it = m_ranges.begin();
        auto i = it->size();
        while (i < index)
        {
            i += it->size();
            ++it;
        }
        index = index - (i - it->size());
        while (count)
        {
            auto x = it->size() - index;
            auto tot = count > x ? x : count;
            count -= tot;
            auto sub = it->subspan(index, tot);
            ret.push_back(sub);
            index = 0;
            ++it;
        }
        return ret;
    }
    void remove_range(std::size_t index, std::size_t count)
    {
        assert(index >= 0 && index < m_size);
        assert(count < m_size || index == 0 && count == m_size);
        m_size -= count;
        auto it = m_ranges.begin();
        auto i = it->size();
        while (i < index)
        {
            i += it->size();
            ++it;
        }
        index = index - (i - it->size());
        if (index)
        {
            auto x = it->size() - index;
            auto tot = count > x ? x : count;
            auto sp = *it;
            auto sub = sp.subspan(0, index);
            *it = sub;
            if (count == tot)
            {
                sub = sp.subspan(index + count);
                it = m_ranges.insert(++it, sub);
            }
            ++it;
            count -= tot;
        }
        while (count)
        {
            auto x = it->size();
            auto tot = count > x ? x : count;
            count -= tot;
            if (tot == it->size())
            {
                it = m_ranges.erase(it);
            }
            else
            {
                auto sub = it->subspan(tot);
                *it = sub;
                ++it;
            }
        }
    }
    void push_back(std::span<int> range)
    {
        m_size += range.size();
        m_ranges.push_back(range);
    }
    void push_front(std::span<int> range)
    {
        m_size += range.size();
        m_ranges.push_front(range);
    }
    std::size_t size() const
    {
        return m_size;
    }
    ranges m_ranges;
    std::size_t m_size = 0x00;
};

template <typename T1, typename T2> struct Pair
{
    T1 first;
    T2 second;
};

enum class Color
{
    Black,
    Red
};

template <typename Ty> struct Node
{
    using node_sptr = std::shared_ptr<Node<Ty>>;
    using value_type = Ty;
    node_sptr m_left;
    node_sptr m_right;
    node_sptr m_parent;
    Color m_color;
    bool m_nil;
    Ty m_value;
    size_t m_size;
};

template <typename Node> struct Iterator
{
    using value_type = typename Node::value_type;
    using reference = value_type &;
    using pointer = value_type *;
    using node_sptr = typename Node::node_sptr;
    reference operator*()
    {
        return m_data->m_value;
    }
    pointer operator->()
    {
        return &(m_data->m_value);
    }
    Iterator() : m_data(nullptr)
    {
    }
    Iterator(node_sptr data) : m_data(data)
    {
    }
    size_t size()
    {
        return m_data->m_size;
    }
    Iterator<Node> &operator++()
    {
        if (!(m_data->m_right->m_nil))
        {
            auto x = m_data->m_right;
            while (!(x->m_left->m_nil))
                x = x->m_left;
            m_data = x;
        }
        else
        {
            auto x = m_data;
            auto y = m_data->m_parent;
            while (!(y->m_nil) && x == y->m_right)
            {
                x = y;
                y = y->m_parent;
            }
            m_data = y;
        }
        return *this;
    }
    Iterator<Node> &operator++(int)
    {
        auto ret = *this;
        ++*this;
        return ret;
    }
    Iterator<Node> &operator--()
    {
        if (!(m_data->m_left->m_nil))
        {
            auto x = m_data->m_left;
            while (!(x->m_right->m_nil))
            {
                x = x->m_right;
            }
            m_data = x;
        }
        else
        {
            auto x = m_data;
            auto y = m_data->m_parent;
            while (!(y->m_nil) && x == y->m_left)
            {
                x = y;
                y = y->m_parent;
            }
            m_data = y;
        }
        return *this;
    }
    Iterator<Node> operator--(int)
    {
        auto ret = *this;
        --*this;
        return ret;
    }
    bool operator==(Iterator<Node> &other)
    {
        return m_data == other.m_data;
    }
    bool operator==(const Iterator<Node> &other) const
    {
        return m_data == other.m_data;
    }
    node_sptr m_data;
};

template <typename Key, typename Value> class OrderStatisticRBtree
{
  public:
    using node_type = Node<Pair<Key, Value>>;
    using node_sptr = typename node_type::node_sptr;
    using iterator = Iterator<node_type>;
    OrderStatisticRBtree()
    {
        init();
    }
    iterator begin()
    {
        if (m_root == m_nil)
        {
            return end();
        }
        return iterator(minimum(m_root));
    }
    iterator end()
    {
        return iterator(m_nil);
    }
    node_sptr minimum(node_sptr x)
    {
        while (x->m_left != m_nil)
            x = x->m_left;
        return x;
    }
    iterator os_search(size_t rank)
    {
        if (m_root == m_nil || rank > m_size)
        {
            return end();
        }
        return _os_search(rank, m_root);
    }
    iterator _os_search(size_t rank, node_sptr node)
    {
        size_t r = node->m_left->m_size + 1;
        if (r == rank)
        {
            return iterator(node);
        }
        if (rank < r)
        {
            return _os_search(rank, node->m_left);
        }
        else
        {
            return _os_search(rank - r, node->m_right);
        }
    }
    void insert(const Key &key, const Value &val)
    {
        auto node = std::make_shared<node_type>();
        node->m_value.first = key;
        node->m_value.second = val;
        node->m_size = 1;
        _insert(node);
        ++m_size;
    }
    iterator erase(iterator it)
    {
        auto x = m_root;
        auto y = m_nil;
        while (x != m_nil)
        {
            y = x;
            x->m_size -= 1;
            if (it->first < x->m_value.first)
            {
                x = x->m_left;
            }
            else if (x->m_value.first < it->first)
            {
                x = x->m_right;
            }
            else
            {
                break;
            }
        }
        if (x != m_nil && x == y)
        {
            auto ret = iterator(x);
            ++ret;
            --m_size;
            _erase(x);
            return ret;
        }
        return m_nil;
    }

    iterator erase(const Key &key)
    {
        auto x = m_root;
        auto y = m_nil;
        while (x != m_nil)
        {
            y = x;
            x->m_size -= 1;
            if (key < x->m_value.first)
            {
                x = x->m_left;
            }
            else if (x->m_value.first < key)
            {
                x = x->m_right;
            }
            else
            {
                break;
            }
        }
        if (x != m_nil && x == y)
        {
            auto ret = iterator(x);
            ++ret;
            --m_size;
            _erase(x);
            return ret;
        }
        return m_nil;
    }

    void _insert(node_sptr z)
    {
        auto x = m_root;
        auto y = m_nil;
        while (x != m_nil)
        {
            y = x;
            x->m_size += z->m_value.size();
            if (z->m_value.first < x->m_value.first)
            {
                x = x->m_left;
            }
            else if (x->m_value.first < z->m_value.first)
            {
                x = x->m_right;
            }
            else
            {
                // equal
                throw;
            }
        }
        z->m_parent = y;
        if (y == m_nil)
        {
            m_root = z;
        }
        else if (z->m_value.first < y->m_value.first)
        {
            y->m_left = z;
        }
        else
        {
            y->m_right = z;
        }
        z->m_left = m_nil;
        z->m_right = m_nil;
        z->m_color = Color::Red;
        fixup_insert(z);
    }

    void fixup_insert(node_sptr z)
    {
        while (z->m_parent->m_color == Color::Red)
        {
            if (z->m_parent == z->m_parent->m_parent->m_left)
            {
                auto y = z->m_parent->m_parent->m_right;
                if (y->m_color == Color::Red)
                {
                    z->m_parent->m_color = Color::Black;
                    y->m_color = Color::Black;
                    z->m_parent->m_parent->m_color = Color::Red;
                    z = z->m_parent->m_parent;
                }
                else
                {
                    if (z == z->m_parent->m_right)
                    {
                        z = z->m_parent;
                        rotate_left(z);
                    }
                    z->m_parent->m_color = Color::Black;
                    z->m_parent->m_parent->m_color = Color::Red;
                    rotate_right(z->m_parent->m_parent);
                }
            }
            else
            {
                auto y = z->m_parent->m_parent->m_left;
                if (y->m_color == Color::Red)
                {
                    z->m_parent->m_color = Color::Black;
                    y->m_color = Color::Black;
                    z->m_parent->m_parent->m_color = Color::Red;
                    z = z->m_parent->m_parent;
                }
                else
                {
                    if (z == z->m_parent->m_left)
                    {
                        z = z->m_parent;
                        rotate_right(z);
                    }
                    z->m_parent->m_color = Color::Black;
                    z->m_parent->m_parent->m_color = Color::Red;
                    rotate_left(z->m_parent->m_parent);
                }
            }
        }
        m_root->m_color = Color::Black;
    }

    void _erase(node_sptr z)
    {
        auto y = z;
        auto y_original_color = y->m_color;
        auto last = z;
        node_sptr x = nullptr;
        if (z->m_left == m_nil)
        {
            x = z->m_right;
            x->m_size = z->m_size;
            transplant(z, z->m_right);
            last = x->m_parent;
        }
        else if (z->m_right == m_nil)
        {
            x = z->m_left;
            x->m_size = z->m_size;
            transplant(z, z->m_left);
            last = x->m_parent;
        }
        else
        {
            y = minimum(z->m_right);
            y_original_color = y->m_color;
            x = y->m_right;
            if (y != z->m_right)
            {
                transplant(y, y->m_right);
                y->m_right = z->m_right;
                y->m_right->m_parent = y;
            }
            else
            {
                x->m_parent = y;
            }
            transplant(z, y);
            y->m_left = z->m_left;
            y->m_left->m_parent = y;
            y->m_color = z->m_color;
            y->m_size = z->m_size;
            last = y;
        }
        auto backdrop = x->m_parent;
        while (backdrop != last)
        {
            --(backdrop->m_size);
            backdrop = backdrop->m_parent;
        }
        if (y_original_color == Color::Black)
        {
            delete_fixup(x);
        }
    }

    void delete_fixup(node_sptr x)
    {
        while (x != m_root && x->m_color == Color::Black)
        {
            if (x == x->m_parent->m_left)
            {
                auto w = x->m_parent->m_right;
                if (w->m_color == Color::Red)
                {
                    w->m_color = Color::Black;
                    x->m_parent->m_color = Color::Red;
                    rotate_left(x->m_parent);
                    w = x->m_parent->m_right;
                }
                if (w->m_left->m_color == Color::Black && w->m_right->m_color == Color::Black)
                {
                    w->m_color = Color::Red;
                    x = x->m_parent;
                }
                else
                {
                    if (w->m_right->m_color == Color::Black)
                    {
                        w->m_left->m_color = Color::Black;
                        w->m_color = Color::Red;
                        rotate_right(w);
                        w = x->m_parent->m_right;
                    }
                    w->m_color = x->m_parent->m_color;
                    x->m_parent->m_color = Color::Black;
                    w->m_right->m_color = Color::Black;
                    rotate_left(x->m_parent);
                    x = m_root;
                }
            }
            else
            {
                auto w = x->m_parent->m_left;
                if (w->m_color == Color::Red)
                {
                    w->m_color = Color::Black;
                    x->m_parent->m_color = Color::Red;
                    rotate_right(x->m_parent);
                    w = x->m_parent->m_left;
                }
                if (w->m_right->m_color == Color::Black && w->m_left->m_color == Color::Black)
                {
                    w->m_color = Color::Red;
                    x = x->m_parent;
                }
                else
                {
                    if (w->m_left->m_color == Color::Black)
                    {
                        w->m_right->m_color = Color::Black;
                        w->m_color = Color::Red;
                        rotate_left(w);
                        w = x->m_parent->m_left;
                    }
                    w->m_color = x->m_parent->m_color;
                    x->m_parent->m_color = Color::Black;
                    w->m_left->m_color = Color::Black;
                    rotate_right(x->m_parent);
                    x = m_root;
                }
            }
        }
        x->m_color = Color::Black;
    }

    void transplant(node_sptr u, node_sptr v)
    {
        if (u->m_parent == m_nil)
        {
            m_root = v;
        }
        else if (u == u->m_parent->m_left)
        {
            u->m_parent->m_left = v;
        }
        else
        {
            u->m_parent->m_right = v;
        }
        v->m_parent = u->m_parent;
    }

    void rotate_left(node_sptr x)
    {
        auto y = x->m_right;
        x->m_right = y->m_left;
        if (y->m_left != m_nil)
        {
            y->m_left->m_parent = x;
        }
        y->m_parent = x->m_parent;
        if (x->m_parent == m_nil)
        {
            m_root = y;
        }
        else if (x == x->m_parent->m_left)
        {
            x->m_parent->m_left = y;
        }
        else
        {
            x->m_parent->m_right = y;
        }
        y->m_left = x;
        x->m_parent = y;

        x->m_size = x->m_left->m_size + x->m_left->m_value.size() + x->m_right->m_size + x->m_right->m_value.size() + 1;
        y->m_size = y->m_left->m_size + y->m_left->m_value.size() + y->m_right->m_size + y->m_right->m_value.size() + 1;
    }

    void rotate_right(node_sptr x)
    {
        auto y = x->m_left;
        x->m_left = y->m_right;
        if (y->m_right != m_nil)
        {
            y->m_right->m_parent = x;
        }
        y->m_parent = x->m_parent;
        if (x->m_parent == m_nil)
        {
            m_root = y;
        }
        else if (x == x->m_parent->m_left)
        {
            x->m_parent->m_left = y;
        }
        else
        {
            x->m_parent->m_right = y;
        }
        y->m_right = x;
        x->m_parent = y;
        x->m_size = x->m_left->m_size + x->m_left->m_value.size() + x->m_right->m_size + x->m_right->m_value.size() + 1;
        y->m_size = y->m_left->m_size + y->m_left->m_value.size() + y->m_right->m_size + y->m_right->m_value.size() + 1;
    }

    void init()
    {
        m_nil = std::make_shared<node_type>();
        m_nil->m_color = Color::Black;
        m_nil->m_nil = true;
        m_nil->m_size = 0;
        m_size = 0;
        m_root = m_nil;
    }

    node_sptr m_root;
    node_sptr m_nil;
    size_t m_size;
};
