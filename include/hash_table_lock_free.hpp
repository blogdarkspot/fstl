#ifndef __HASH_TABLE_LOCK_FREE__
#define __HASH_TABLE_LOCK_FREE__
#include <utility>
#include <atomic>

template<typename NodeT>
struct forward_iterator_list {
	using iterator_category = std::forward_iterator_tag;
	using value_type = typename NodeT::value_type;
	using reference = value_type&;
	using difference_type = std::ptrdiff_t;

	forward_iterator_list() : _M_data(nullptr) {}

	explicit forward_iterator_list(NodeT* data)   : _M_data(data) {}

	reference operator*() {
		return _M_data->_M_value;
	}

	forward_iterator_list<NodeT>& operator++()
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
	
	bool operator!=(const forward_iterator_list<NodeT>& rhs) const
	{
		return _M_data != rhs._M_data;
	}

	bool operator==(const forward_iterator_list<NodeT>& rhs) const
	{
		return (this->_M_data == rhs._M_data);
	}

	NodeT *get_unsafe_pointer()
	{
        return _M_data;
	}
	
	private:

	NodeT* _M_data;
};

template<typename T>
struct forward_node {
	using value_type = T;
	using pointer = T*;
	using reference  = T&;

	forward_node()  = default;

	value_type  _M_value;
	forward_node<T>* _M_next;
    std::atomic<bool> _M_deleted = false;
	bool _M_nil = false;

	template<typename Allocator>
	static forward_node<T>* allocate(Allocator& allocator)
	{
		auto node = std::allocator_traits<Allocator>::allocate(allocator, 1);
        std::allocator_traits<Allocator>::construct(allocator, node);
		return node;
	}
	
	template<typename Allocator>
	static void deallocate(Allocator& allocator, forward_node<T>* ptr)
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
template<typename T, 
		typename TEqual = std::equal_to<T>, 
		typename Allocator = std::allocator<T>>
class set_lf {
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
	using node_ptr = node_type*;
	using allocator_node = typename alloc_traits::template rebind_alloc<node_type>;

	set_lf() 
	{
		_M_end.store(node_type::allocate(_M_allocator));
        _M_end.load()->_M_nil = true;
        _M_begin = _M_end.load();
	}

	~set_lf()
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

	iterator find(const T& value)
	{
		auto ret = begin();
		auto last = end();
		while(ret != last && *ret != value) {
			++ret;
		}
		return ret;
	}

	template<bool force=true>
	iterator insert(T value)
	{
		//start point
		node_ptr orig_head = _M_begin.load();
		//step-1 Check if the value already exists in the list;
		{
			auto start = orig_head; 
			auto last = _M_end.load();
			while(start != last && start->_M_value != value) 
			{
				start = start->_M_next;
			}
			if(start != last)
			{
				//found
				if(force)
				{
					start->_M_value = value;
				}
				return iterator(start);
			}
		}		
		//step-2 allocate new node and try insert into begin of the list
		auto new_node =  node_type::allocate(_M_allocator);	
		new_node->_M_next = orig_head;
        new_node->_M_value = value;
		while(!_M_begin.compare_exchange_strong(new_node->_M_next, new_node))
		{
			//step-3 this case only occurs when the head was change by another
			//thread, so we need first update our begin reference,
			//try find if the new node is equals to the value and try again if necessary

			auto head = new_node->_M_next;
			while(head != _M_end && head != orig_head )
			{
				
				if(TEqual{}(head->_M_value, value))
				{
					if(force)
					{
						head->_M_value = value;
					}
					//discart the new node
					node_type::deallocate(_M_allocator, new_node);
					return iterator(head);
				}
				head = head->_M_next;
			}
			orig_head = new_node->_M_next;
		}
        ++_M_count;
		return iterator(new_node);
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
        return ret;
	} 

private:
	std::atomic<node_type*> _M_begin;
	std::atomic<node_type*> _M_end;
    std::atomic<size_type> _M_count;
	allocator_node _M_allocator;
};

template<class KeyT, class ValueT, 
		class HashFuncT = std::hash<KeyT>, 
		class AllocatorT = std::allocator<std::pair<KeyT, ValueT>>>
class hash_table_lock_free {

	using value_type = std::pair<KeyT, ValueT>;
	using allocator_type = AllocatorT;
	using alloc_traits = std::allocator_traits<allocator_type>;

	struct equal_value_type{
		bool operator()(const value_type& lhs, const value_type& rhs)
		{
			return lhs.first == rhs.first;
		}

	};

	using iterator = typename set_lf<value_type,
						equal_value_type,
						allocator_type>::iterator;

	struct equal_iterator_type {
		bool operator()(const iterator& lhs, const iterator& rhs)
		{
			return lhs == rhs;
		}
	};
	using bucket_type = set_lf<iterator, 
						equal_iterator_type, 
						allocator_type>;
	using vec_bucket = bucket_type*;

	struct properties
	{
		properties() : _M_size(0x00), _M_count(0x00), _M_lock(false),
		_M_table(nullptr), _M_hash() {}
		
		size_t _M_size;
		size_t _M_count;
		std::atomic<bool> _M_lock;
		vec_bucket _M_table;		
		HashFuncT _M_hash;

		template<typename AllocatorT1>
		static properties* allocate(AllocatorT1& allocator)
		{
			auto table = std::allocator_traits<AllocatorT1>::allocate(allocator, 1);
			::new (static_cast<void *>(table)) properties();
			return table;
		}

		template<typename AllocatorT1>
		static void initialize_table(AllocatorT1& allocator, properties& table, size_t size)
		{
			using allocator_chain = typename std::allocator_traits<AllocatorT1>::template rebind_alloc<bucket_type>;
			auto chain = std::allocator_traits<allocator_chain>::allocate(allocator, size);
			table._M_table =  chain;
			table._M_size = size;
			table._M_count = 0x00;
			for(int i = 0; i < table._M_size; ++i)
			{
				::new(static_cast<void *>(&(table._M_table[i]))) bucket_type();
			}	
		}

		template<typename AllocatorT1>
		static void deallocate(AllocatorT& allocator, properties* ptr)
		{
		};
	};

	using alloc_properties = typename alloc_traits::template rebind_alloc<properties>;
	using alloc_bucket = typename alloc_traits::template rebind_alloc<bucket_type>;



	public:

	hash_table_lock_free() 
	{
		constexpr unsigned int init_size = (1 << 4);
		_M_table.store(properties::allocate(_M_allocator_table));
	}

	~hash_table_lock_free()
	{
	}

	void try_resize()
	{
		auto ptr = _M_table.load();
		bool expected = false;
		if(ptr->_M_lock.compare_exchange_strong(expected, true))
		{
			auto new_table = properties::allocate(_M_allocator_table);
			new_table->_M_lock.store(true);
			_M_table.store(new_table);
			/*
			for(auto& row : ptr->_M_table)
			{
				for(auto& value : row)
				{
					try_insert(value);
				}	
			}*/
			new_table->_M_lock.store(false);
		}
	};

	iterator insert(value_type value)
	{
		constexpr float max_load_factor = 0.7F;
		auto ptr = _M_table.load();
		auto index = calculate_index(ptr->_M_hash, value.first, ptr->_M_size);
		iterator it;	
		ptr->_M_table[index].insert(it);		
		
		if(ptr != _M_table.load())
		{
			insert(value);
		}
		if(load_factor() >= max_load_factor)
		{
			try_resize();	
		}
		return it;
	}

	float load_factor()
	{
		auto ptr = _M_table.load();
		return (float)ptr->_M_count / (float)ptr->_M_size;
	}

	void resize()
	{
	}

	void isert_or_update(iterator& it)
	{
	 	auto ptr = _M_table.load();
		auto index = calculate_index(ptr->_M_hash, it.first, ptr->_M_size);
		
	}		

	void try_insert(iterator& it)
	{
	}

	size_t calculate_index(HashFuncT& hash, const KeyT& key, size_t size)
	{
		return hash(key) % size;
	};

	
private:	
	std::atomic<properties*> _M_table;
	alloc_properties _M_allocator_table;
	alloc_bucket _M_alloc_bucket;
};
#endif
