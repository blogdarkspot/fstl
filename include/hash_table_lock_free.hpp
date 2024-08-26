#ifndef __HASH_TABLE_LOCK_FREE__
#define __HASH_TABLE_LOCK_FREE__
#include <utility>
#include <atomic>

namespace lf
{

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

/**
*
    Esta é uma lista lock-free e wait free que tem como proposito
servir de corrente para resolução de conflitos em uma hash table. Sendo
assim o comportamento desta lista se resume a inserir um item no começo
da lista, buscar por um item na lista e remover um item da lista. Como
o objetivo é ser uma lista de objetos únicos para a hash table caso
dois items de mesmo valor sejam inseridos na fila ao mesmo tempo o que conseguir
completar todo o processo primeiro terá prioridade, somente neste momento o item de mesmo
valor é checado, fora este cenário itens de mesmo valor podem ser inseridos normalmente.

*/
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

    std::pair<iterator, bool> insert(T value)
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
                return {iterator(start), false};
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

                if (TEqual{}(head->_M_value, value))
                {
                    node_type::deallocate(_M_allocator, new_node);
                    return {iterator(head), false};
                }
                head = head->_M_next;
            }
            orig_head = new_node->_M_next;
        }
        ++_M_count;
        return {iterator(new_node), true};
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

    reference operator[](int index)
    {
        return _M_buckets[index];
    }

    size_type size() const
    {
        return _M_size;
    }

    bool lock()
    {
        bool ret = true;
        return _M_lock.compare_exchange_strong(ret, false) ? true : false;
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

    explicit VecTable(size_type size) : _M_size(size), _M_lock(false), _M_buckets(nullptr)
    {
        _M_buckets = CreateVecTable(_M_alloc_bucket, _M_size);
    }

    ~VecTable()
    {
        DestroyVecTable(_M_alloc_bucket, _M_buckets, _M_size);
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
    std::atomic<bool> _M_lock;
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

    HashTable() : _M_capacty(1 << 5), 
                _M_size(0x00),
                _M_bucket(nullptr), _M_old_bucket(nullptr)
    {
        _M_bucket.store(new bucket_type(_M_capacty));
    }

    void insert(value_type value)
    {
        auto index = get_index(value.first);
        auto bucket = _M_bucket.load();

        (*bucket)[index].insert(value.first);

        if (_M_bucket.load() != bucket)
        {
            insert(value);
        }
        else
        {
            if (load_factor() >= _M_max_load_factor)
            {
                bucket_type* null = nullptr;
                if (_M_old_bucket.compare_exchange_strong(null, bucket))
                {
                    auto capacity = _M_capacty << 1;
                    auto new_bucket = new bucket_type(capacity);
                    _M_bucket.store(new_bucket);
                    _M_capacty = capacity;
                    rehash();
                }
            }
        }
    }

    size_type size()
    {
        return _M_size;
    }

    float load_factor()
    {
        auto bucket = _M_bucket.load();
        return (float)bucket->size() / size();
    }
    
  private:

    size_type get_index(Key key)
    {
        auto hash = _M_hasher(key);
        auto bucket = _M_bucket.load();
        return hash % bucket->size();
    }

    void rehash()
    {

    }

    constexpr static float _M_max_load_factor = 0.8f;
    size_type _M_capacty;
    size_type _M_size;
    std::atomic<bucket_type*> _M_bucket;
    std::atomic<bucket_type*> _M_old_bucket;
    HashFunc _M_hasher;
};
} // namespace lf

/*

template <typename T, typename Allocator = std::allocator<T>> struct forward_list
{
    using node_type = forward_node<T>;
    using iterator = forward_iterator_list<node_type>;

    iterator insert(forward_node<T> *node)
    {
        auto orign_begin = _M_begin.load();
        node->_M_next = orign_begin;
        while (!_M_begin.compare_exchange_strong(orign_begin, node))
        {
            node->_M_next = orign_begin;
        }
        return iterator(node);
    }

    void erase(iterator &value)
    {
    }

    iterator begin()
    {
        return iterator(_M_begin.load());
    }

    iterator end()
    {
        return iterator(_M_end.load());
    }

  private:
    std::atomic<node_type *> _M_begin;
    std::atomic<node_type *> _M_end;
};

template<class KeyT, class ValueT, 
		class HashFuncT = std::hash<KeyT>, 
		class AllocatorT = std::allocator<std::pair<KeyT, ValueT>>>
class htable_lf {
    
	using value_type = std::pair<KeyT, ValueT>;
	using key_type = KeyT;
	using mapped_type = ValueT;
	using allocator_type = AllocatorT;
	using alloc_traits = std::allocator_traits<allocator_type>;
	using table_type = htable<KeyT, ValueT, HashFuncT, AllocatorT>;
	using alloc_table = typename alloc_traits::template rebind_alloc<table_type>;
	using size_type = typename alloc_traits::size_type;
	using iterator =  typename table_type::container_type::iterator;
	using container_type = typename table_type::container_type;


	public:

	htable_lf() 
	{
		constexpr unsigned int init_size = (1 << 4);
        _M_table.store(new table_type(init_size));
        _M_old_table.store(_M_table);
	}

	~htable_lf()
	{
	}

	float load_factor()
	{
        
		auto ptr = _M_table.load();
		return (float)ptr->_M_count / (float)ptr->_M_size;
		
	}

	float max_load_factor()
	{
        constexpr float max = 0.8F;
        return max;
	}

	std::pair<iterator, bool> insert(value_type value)
	{
        auto orig_table = _M_table.load();
        auto index = calculate_index(orig_table->_M_hasher, value.first, orig_table->_M_size);
        auto node = new forward_node<value_type>();
        node->_M_value = value;
        node->_M_ready = false;
        auto it = iterator(node);
        auto ret = orig_table->_M_buckets[index].insert(it);

		if (!ret.second)
		{
            delete node;
            return {*(ret.first), false};
		}

		_M_container.insert(node);

        node->_M_ready = true;

		if (orig_table != _M_table.load())
		{
            return insert(value);
		}

        ++(orig_table->_M_count);

		try_rehash();

        return {it, true};
	}

	void try_rehash()
	{
		if (load_factor() >= max_load_factor())
		{
            bool try_lock = false;
			if (_M_table.load()->_M_lock.compare_exchange_strong(try_lock, true))
			{
                auto orig_table = _M_table.load();
                _M_old_table.store(orig_table);
                auto new_table = table_type::allocate(_M_alloc_table, (_M_table.load()->_M_size) << 1);
                
				if (!_M_table.compare_exchange_strong(orig_table, new_table))
				{
					//PANIC não deveria acontecer
				}
                auto old_table = _M_old_table.load();
                size_type old_size = old_table->_M_size;
                size_type old_count = old_table->_M_count;

				for (size_type i = 0; i < old_size; ++i)
				{
                    auto start = old_table->_M_buckets[i].begin().get_unsafe_pointer();
                    auto last = old_table->_M_buckets[i].end().get_unsafe_pointer();
					while (start != last)
					{
						if (!start->_M_deleted)
						{
//                            insert()
						}
					}
				}
			}
		}
	}

	std::pair<iterator, bool> isert_or_assing(const key_type& key, mapped_type&& value)
	{
        auto ret = insert({key, value});
		if (!ret.second)
		{
            auto it = ret.first;
            auto ptr = it.get_unsafe_pointer();
            ptr->_M_value = value;
		}
        return {ret.first, true };
	}		

	size_t calculate_index(HashFuncT& hash, const KeyT& key, size_t size)
	{
		return hash(key) % size;
	};

	
private:	
	std::atomic<table_type*> _M_table;
	std::atomic<table_type*> _M_old_table;
    alloc_table _M_alloc_table;
    container_type _M_container;

};
*/
#endif
