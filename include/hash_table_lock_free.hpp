#ifndef __HASH_TABLE_LOCK_FREE__
#define __HASH_TABLE_LOCK_FREE__
#include <utility>
#include <list>
#include <vector>
#include <iostream>

template<typename NodeT>
struct iterator_list {
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = typename NodeT::value_type;
	using reference = value_type&;
	using difference_type = std::ptrdiff_t;

	iterator_list() : _M_data(nullptr) {}

	iterator_list(NodeT* data) : _M_data(data) {}

	reference operator*() {
		return _M_data->_M_value;
	}

	iterator_list& operator--()
	{
		_M_data =  _M_data->_M_parent;
		return *this;
	}

	iterator_list operator--(int)
	{
		auto ret = *this;
		--*this;
		return ret;
	}

	iterator_list& operator++()
	{
		_M_data = _M_data->_M_next;
		return *this;
	}
	
	iterator_list operator++(int)
	{
		auto ret = *this;
		--*this;
		return ret;
	}	
	
	bool operator!=(const iterator_list& rhs)
	{
		return _M_data == rhs._M_data;
	}

	private:
	NodeT* _M_data;
};

template<typename T>
struct node_list {
	using value_type = T;
	using pointer = T*;
	using reference  = T&;

	node_list()  = default;

	value_type*  _M_value;
	node_list<T>* _M_parent;
	node_list<T>* _M_next;
	bool _M_nil;
	
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
template<typename T>
class list_lock_free {
public:
	using node_type = node_list<T>;
	using node_ptr = node_type*;
	using iterator = iterator_list<node_type>;

	list_lock_free() 
	{
	//	_M_end.store(new node_type());
	//	_M_begin.store(new node_type());
		//_M_begin->_M_next.store(_M_end);
		//_M_begin->_M_parent.store(_M_end);
		//_M_end->_M_parent.store(_M_begin);
	}

	list_lock_free(list_lock_free<T>&& other) :
		_M_begin(other._M_begin.load()),
		_M_end(other._M_end) {
		other._M_begin = nullptr;
		other._M_end  = nullptr;
	}
	list_lock_free<T>& operator=(const list_lock_free<T>& other)
	{
		_M_begin.store(other._M_begin.load());
		_M_end.store(other._M_end.load());
		return *this;
	}

	iterator begin()
	{
		return iterator(_M_begin.load());
	}
	
	iterator end()
	{
		return iterator(_M_end.load());
	}
	/*
		
	*/	
	iterator insert(T value)
	{
		node_ptr beg = _M_begin.load();
		node_ptr new_node = new node_type();	
		new_node->_M_next = beg;
		while(!_M_begin.compare_exchange_strong(new_node->_M_next, new_node))
		{
			//agora verificamos se algum valor igual jã foi inserido
			//agora new_node->_M_next ~e a atual header 
			//entao pode existir um range de nós entre _M_next - beg
			auto head = new_node->_M_next;
			while(head != _M_end)
			{
				/*
				if(head->_M_value == value)
				{
					return iterator(head);
				}*/
			}
			//o nó que iriamos colocar foi removido
			//então começamos novamente e sabemos que 
			//este valor ainda não foi inserido
			if(head == _M_end)
			{
				
			}
		}
		return iterator(_M_end.load());
	}

	iterator try_insert(T value)
	{
		
		return iterator(_M_end.load());
	}
	
	iterator erase(T value)
	{
		return iterator(_M_end.load());
	} 

	void erase(iterator value)
	{
	}
private:
	std::atomic<node_type*> _M_begin;
	std::atomic<node_type*> _M_end;
};

template<class KeyT, class ValueT, class HashFuncT = std::hash<KeyT>>
class hash_table_lock_free {

	using value_type = std::pair<KeyT, ValueT>;
	using iterator = typename list_lock_free<value_type>::iterator;
	using chain_type = list_lock_free<iterator>;
	using bulk = chain_type*;

	struct hash_table_properties
	{
		size_t _M_size;
		size_t _M_count;
		std::atomic<bool> _M_lock;
		bulk _M_table;		
		HashFuncT _M_hash;

		static hash_table_properties* create_table(size_t size)
		{
			hash_table_properties* table = new hash_table_properties();
			initialize_table(*table, size);	
			return table;

		}

		static void initialize_table(hash_table_properties& table, size_t size)
		{
			table._M_table = new  chain_type[size];
			table._M_size = size;
			table._M_count = 0x00;
			for(int i = 0; i < table._M_size; ++i)
			{
				table._M_table[i] = std::move(chain_type());
			}	
		}
	};



	public:

	hash_table_lock_free() 
	{

		_M_table.store(hash_table_properties::create_table(1<<4));
		if(_M_table.is_lock_free())
		{
			std::cout << "is lock free" << std::endl;
		}
		else
		{
			std::cout << "is not lock free" << std::endl;
		}
	}

	void try_resize()
	{
		auto ptr = _M_table.load();
		bool expected = false;
		if(ptr->_M_lock.compare_exchange_strong(expected, true))
		{
			hash_table_properties* new_table = 	
				hash_table_properties::create_table(ptr->_M_size << 1);
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
		auto ptr = _M_table.load();
		auto index = calculate_index(ptr->_M_hash, value.first, ptr->_M_size);
		iterator it;	
		ptr->_M_table[index].insert(it);		
		
		if(ptr != _M_table.load())
		{
			insert(value);
		}
		if(load_factor() >= 0.7f)
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

	std::atomic<hash_table_properties*> _M_table;
};
#endif
