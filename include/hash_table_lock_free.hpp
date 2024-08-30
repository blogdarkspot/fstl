#ifndef __HASH_TABLE_LOCK_FREE__
#define __HASH_TABLE_LOCK_FREE__
#include <utility>
#include <atomic>
#include <iostream>
#include "memory.hpp"

namespace lf
{
/*
   beg      _a_      _b_      _c_      end
  |   |--->|   |--->|   |--->|   |--->|   |
  |___|<---|___|<---|___|<---|___|<---|___|
    
    remoção:
    cenário 1: removendo a e b em paralelo:
      iniciando por b pegamos a->b e ligamos a->c,
      em paralelo a faz beg->a to beg->b
*/

template <typename NodeT> struct forward_iterator_list
{
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename NodeT::value_type;
    using reference = value_type &;
    using difference_type = std::ptrdiff_t;

    forward_iterator_list() : _M_data(nullptr)
    {
    }

    explicit forward_iterator_list(NodeT *data) : _M_data(data)
    {
    }

    reference operator*()
    {
        return _M_data->_M_value;
    }

    forward_iterator_list<NodeT> &operator++()
    {
        _M_data = _M_data->_M_next;
        while ((!_M_data->_M_nil) && _M_data->_M_deleted)
        {
            _M_data = _M_data->_M_next;
        }
        return *this;
    }

    forward_iterator_list<NodeT> operator++(int)
    {
        auto ret = *this;
        ++*this;
        return ret;
    }

    bool operator!=(const forward_iterator_list<NodeT> &rhs) const
    {
        return _M_data != rhs._M_data;
    }

    bool operator==(const forward_iterator_list<NodeT> &rhs) const
    {
        return (this->_M_data == rhs._M_data);
    }

    NodeT *get_unsafe_pointer()
    {
        return _M_data;
    }

  private:
    NodeT *_M_data;
};

template <typename T> struct forward_node
{
    using value_type = T;
    using pointer = T *;
    using reference = T &;

    forward_node() = default;

    value_type _M_value;
    forward_node<T> *_M_next;
    forward_node<T> *_M_previous;
    std::atomic<bool> _M_deleted = false;
    bool _M_ready = false;
    bool _M_nil = false;

    template <typename Allocator> static forward_node<T> *allocate(Allocator &allocator)
    {
        auto node = std::allocator_traits<Allocator>::allocate(allocator, 1);
        std::allocator_traits<Allocator>::construct(allocator, node);
        return node;
    }

    template <typename Allocator> static void deallocate(Allocator &allocator, forward_node<T> *ptr)
    {
        std::allocator_traits<Allocator>::destroy(allocator, ptr);
        std::allocator_traits<Allocator>::deallocate(allocator, ptr, 1);
    }
};

template <typename T, typename Allocator = std::allocator<T>> class list
{
  public:
    using value_type = T;
    using node_type = forward_node<T>;
    using allocator_type = Allocator;
    using alloc_traits = std::allocator_traits<allocator_type>;
    using allocator_node = typename alloc_traits::template rebind_alloc<node_type>;
    using iterator = forward_iterator_list<node_type>;

    list()
    {
        _M_end.store(node_type::allocate(_M_allocator_node));
        _M_begin.store(_M_end.load());
    }

    iterator append(const value_type& value)
    {
        auto nnode_ = node_type::allocate(_M_allocator_node); 
        nnode_->_M_value = value;

        auto begin_ = _M_begin.load();
        nnode_->_M_next = begin_;
        while (!_M_begin.compare_exchange_strong(begin_, nnode_))
        {
            nnode_->_M_next = begin_;
        }
        return iterator(nnode_);
    }

    void erase(iterator& it)
    {
        node_type* node_ = it.get_unsafe_pointer();
        node_->_M_deleted.store(true);
    }

    iterator begin()
    {
        auto begin = _M_begin.load();
        return iterator(begin);
    }

    iterator end()
    {
        auto end = _M_end.load();
        return iterator(end);
    }

    ~list()
    {
        while (_M_begin != _M_end)
        {
            auto begin = _M_begin.load();
            _M_begin.store(begin->_M_next);
            node_type::deallocate(_M_allocator_node, begin);
        }
        node_type::deallocate(_M_allocator_node, _M_end.load());
    }

  private:
    std::atomic<node_type *> _M_begin;
    std::atomic<node_type *> _M_end;
    allocator_node _M_allocator_node;
};


template <typename T, typename TEqual = std::equal_to<T>, typename Allocator = std::allocator<T>> 
class set
{
  public:
    using node_type = forward_node<T>;

    using value_type = T;
    using allocator_type = Allocator;
    using alloc_traits = std::allocator_traits<allocator_type>;

    using pointer = typename alloc_traits::pointer;
    using const_pointer = typename alloc_traits::const_pointer;
    using size_type = typename alloc_traits::size_type;
    using difference_type = typename alloc_traits::difference_type;

    using iterator = forward_iterator_list<node_type>;
    using node_ptr = node_type *;
    using allocator_node = typename alloc_traits::template rebind_alloc<node_type>;

    set()
    {
        _M_end.store(node_type::allocate(_M_allocator));
        _M_end.load()->_M_nil = true;
        _M_begin = _M_end.load();
    }

    ~set()
    {
        auto start = _M_begin.load();
        auto last = _M_end.load();
        while (start != last)
        {
            auto next = start->_M_next;
            node_type::deallocate(_M_allocator, start);
            start = next;
        }
        node_type::deallocate(_M_allocator, last);
    }

    iterator begin()
    {
        auto ret = iterator(_M_begin.load());
        if (_M_begin.load()->_M_deleted)
        {
            ++ret;
        }
        return ret;
    }

    iterator end()
    {
        return iterator(_M_end.load());
    }

    size_type size() const
    {
        return _M_count.load();
    }

    iterator find(const T &value)
    {
        auto ret = begin();
        auto last = end();
        while (ret != last && *ret != value)
        {
            ++ret;
        }
        return ret;
    }

    size_t insert(T value)
    {
        
        // start point
        node_ptr orig_head = _M_begin.load();
        // step-1 Check if the value already exists in the list;
        {
            auto start = orig_head;
            auto last = _M_end.load();
            while (start != last && start->_M_value != value)
            {
                start = start->_M_next;
            }
            if (start != last)
            {
                return 0;
            }
        }
        // step-2 allocate new node and try insert into begin of the list
        auto new_node = node_type::allocate(_M_allocator);
        new_node->_M_next = orig_head;
        new_node->_M_value = value;
        while (!_M_begin.compare_exchange_strong(new_node->_M_next, new_node))
        {
            // step-3 this case only occurs when the head was change by another
            // thread, so we need first update our begin reference,
            // try find if the new node is equals to the value and try again if necessary

            auto head = new_node->_M_next;
            while (head != _M_end && head != orig_head)
            {

                if (head->_M_value == value)
                {
                    node_type::deallocate(_M_allocator, new_node);
                    return 0;
                }
                head = head->_M_next;
            }
            orig_head = new_node->_M_next;
        }
        ++_M_count;
        return 1;
    }

    iterator erase(T value)
    {
        auto ret = find(value);
        if (ret == end())
        {
            return ret;
        }
        auto raw = ret.get_unsafe_pointer();
        bool expected = false;
        if (raw->_M_deleted.compare_exchange_strong(expected, true))
        {
            --_M_count;
        }
        else
        {
            return end();
        }
        ++ret; // return the next one;
        return ret;
    }

  private:
    std::atomic<node_type *> _M_begin;
    std::atomic<node_type *> _M_end;
    std::atomic<size_type> _M_count;
    allocator_node _M_allocator;
};



/*
 */
template <class T, class Allocator = std::allocator<T>> struct VecTable
{
    using value_type = T;
    using allocator_type = Allocator;
    using reference = value_type &;

    using alloc_traits = std::allocator_traits<allocator_type>;
    using size_type = typename alloc_traits::size_type;
    using difference_type = typename alloc_traits::difference_type;
    using pointer = typename alloc_traits::pointer;
    using const_pointer = typename alloc_traits::const_pointer;

    enum class State : std::uint8_t
    {
        LOCKED = 0,
        UPDATING = 1,
        READY = 2
    };

    reference operator[](int index)
    {
        return _M_buckets[index];
    }

    size_type size() const
    {
        return _M_size;
    }

    size_type capacity() const
    {
        return _M_capacity;
    }

    bool lock()
    {
        auto ready = State::READY;
        return _M_state.compare_exchange_strong(ready, State::LOCKED);
    }

    bool update()
    {
        auto locked = State::LOCKED;
        return _M_state.compare_exchange_strong(locked, State::UPDATING);
    }

    bool ready()
    {
        auto updating = State::UPDATING;
        return _M_state.compare_exchange_strong(updating, State::READY);
    }

    State state()
    {
        return _M_state.load();
    }

    template <typename Alloc, typename... Params> static VecTable *allocate(Alloc allocator, Params &&...params)
    {
        auto ptr = std::allocator_traits<Alloc>::allocate(allocator, 1);
        new (static_cast<void *>(ptr)) htable(std::forward<Params>(params)...);
        return ptr;
    }

    template <typename Alloc> static void deallocate(Alloc allocator, pointer ptr)
    {
        std::allocator_traits<Alloc>::destroy(allocator, ptr);
        std::allocator_traits<Alloc>::deallocate(allocator, ptr, 1);
    }

    explicit VecTable(size_type capacity) :_M_capacity(capacity), _M_size(capacity), _M_state(State::LOCKED), _M_buckets(nullptr)
    {
        _M_buckets = CreateVecTable(_M_alloc_bucket, _M_size);
    }

    VecTable(VecTable& other, const size_type capacity)
    {
        _M_capacity = capacity;
        _M_size = capacity;
        pointer ret = alloc_traits::allocate(_M_alloc_bucket, capacity);
        _M_buckets = ret;
        const auto &old_capacity = other._M_capacity;
        for (size_type i = 0; i < old_capacity; ++i)
        {
            ret = std::addressof(other[i]);
            ++ret;
        }
        const auto& rest = capacity - old_capacity;
        for (size_type i = 0; i < rest; ++i)
        {
            alloc_traits::construct(allocator, std::addressof(ret[i]));
        }

    }

    void release()
    {
        DestroyVecTable(_M_alloc_bucket, _M_buckets, _M_size);
    }

    ~VecTable()
    {
    }

  private:
    pointer CreateVecTable(allocator_type &allocator, size_type size)
    {
        pointer ret = alloc_traits::allocate(allocator, size);
        for (size_type i = 0; i < size; ++i)
        {
            alloc_traits::construct(allocator, std::addressof(ret[i]));
        }
        return ret;
    }


    void DestroyVecTable(allocator_type &allocator, pointer buckets, size_type size)
    {
        for (size_type i = 0; i < size; ++i)
        {
            alloc_traits::destroy(allocator, std::addressof(buckets[i]));
        }
        alloc_traits::deallocate(allocator, buckets, size);
    }

    allocator_type _M_alloc_bucket;
    std::atomic<State> _M_state = State::LOCKED;
    size_type _M_size;
    size_type _M_capacity;
    pointer _M_buckets;
};

template<typename Key, typename Value, 
        typename HashFunc = std::hash<Key>,
        typename Compare = std::equal_to<Key>,
        typename Allocator = std::allocator<std::pair<Key, Value>>>
class HashTable
{
  public:
    using value_type = std::pair<Key, Value>; 
    using allocator = Allocator;
    
    using set_type = typename set<Key, Compare>;
    using bucket_type = typename VecTable<set_type>;
    using size_type = typename bucket_type::size_type;
    using bucket_wrap_ptr = memory::WrapPtr<bucket_type>;

    using weak_bucket_ptr = memory::weak_unique_ptr<bucket_type>;

    HashTable()  
    {
        _M_bucket1.reset(new bucket_type(1 << 18));
        auto bucket = _M_bucket1.try_acquire();
        bucket->update();
        bucket->ready();

        _M_current_bucket.store(&_M_bucket1);
        _M_old_bucket.store(&_M_bucket2);
    }

    void insert(value_type value)
    {
        const auto use_bucket1 = _M_use_bucket1.load();
        auto& bucket = use_bucket1 ? _M_bucket1 : _M_bucket2;
        auto vec = bucket.try_acquire();
        const auto capacity = vec->capacity();
        auto index = get_index(value.first, capacity);
        auto ret = (*vec)[index].insert(value.first);
        
        if (_M_use_bucket1.load() != use_bucket1)
        {
            bucket.release();
            insert(value);
        }
        else
        {
           
            if (load_factor(capacity) >= _M_max_load_factor)
            {
                if (vec->lock())
                {
                    auto& oldbucket = use_bucket1 ? _M_bucket2 : _M_bucket1;
                    auto new_bucket = new bucket_type(bucket, capacity << 1);
                    auto ret = oldbucket.reset(new_bucket);
                    new_bucket->update();
                    _M_use_bucket1.store(!use_bucket1);
                    rehash(vec, new_bucket);
                    new_bucket->ready();
                    bucket.release(); //segundo release para limpar o container
                }
            }
        }
         
        bucket.release();
        _M_size.fetch_add(ret);
    }

    void find(const Key& key)
    {
        auto bucket = _M_current_bucket.load();
        auto vec = bucket->try_acquire();
        if (vec)
        {
            const auto capacity = vec->capacity();
            auto index = get_index(key, capacity);
            auto ret = (*vec)[index].find(key);
            if (!ret.second && (*vec).state() == bucket_type::State::UPDATING)
            {
                bucket->release();
                bucket = _M_old_bucket.load();
                vec = bucket->try_acquire();
                if (vec)
                {
                    index = get_index(key, vec->capacity());
                    ret = (*vec)[index].find(key);
                    bucket->relase();
                }
            }
        }
    }

    size_type size()
    {
        return _M_size.load();
    }


    inline const float load_factor(const float capacity) const
    {
        return _M_size.load() / capacity;
    }
    
  private:

    inline size_type get_index(const Key& key, const size_type capacity)
    {
        auto hash = _M_hasher(key);
        return hash % capacity;
    }

    void rehash(bucket_type* _old, bucket_type* _new)
    {
        auto size = _old->capacity();
        auto new_capacity = _new->capacity();
        for (size_type i = 0; i < size; ++i)
        {
            auto start = (*_old)[i].begin();
            auto end = (*_old)[i].end();
            
            for (; start != end; ++start)
            {
                auto index = get_index(*start, new_capacity);
                (*_new)[index].insert(*start);
            }
        }

    }

    constexpr static float _M_max_load_factor = 0.5f;
    std::atomic_uint64_t _M_size = 0;
    HashFunc _M_hasher;
    bucket_wrap_ptr _M_bucket1;
    bucket_wrap_ptr _M_bucket2;
    std::atomic_bool _M_use_bucket1 = true;
    std::atomic<bucket_wrap_ptr*> _M_current_bucket;
    std::atomic<bucket_wrap_ptr*> _M_old_bucket;
    
};
} // namespace lf
#endif
