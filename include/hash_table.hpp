#ifndef __HASH_TABLE__
#define __HASH_TABLE__

#include<vector>
#include<list>

template<typename KeyT, typename ValueT, typename HashFuncT = std::hash<KeyT>>
struct xhash_table {
	using value_type = std::pair<KeyT, ValueT>;
	using list_type = std::list<value_type>;
	using iterator = typename list_type::iterator;
	using bulk_type = std::vector<std::list<iterator>>;

	xhash_table()
	{
		_M_size = 1 << 4;
		initialize_bulk(_M_table, _M_size);	
	};

	void initialize_bulk(bulk_type& bulk, size_t size)
	{
		bulk.reserve(size);
		for(int i = 0; i < size; ++i)
		{
			std::list<iterator> empty;
			bulk.emplace_back(empty);
		}
	}

	void rehash(size_t new_size)
	{
		bulk_type bulk;
		initialize_bulk(bulk, new_size);
		auto begin = _M_values.begin();
		auto end = _M_values.end();

		while(begin != end)
		{
			auto index = find_index(begin.first, new_size);
			_M_table[index].emplace_back(begin);
			++begin;
		}
	}

	iterator insert(value_type value)
	{
		auto index = find_index(value.first, _M_size);

		if(!_M_table[index].empty())
		{

			for(const auto& pos : _M_table[index])
			{
				if((pos)->first == value.first)
				{

					return pos;
				}
			}

		}

		iterator it = _M_values.insert(_M_values.begin(), value);
		_M_table[index].emplace_back(it);
		return it;
	}

	iterator erase(KeyT key)
	{
		auto index = find_index(key, _M_size);
		if(!_M_table[index].empty())
		{
			auto end = _M_table[index].end();

			for(auto it = _M_table[index].begin() ; it != end; ++it)
			{
				if((*it)->first == key)
				{
					auto ret = _M_values.erase(*it);
					_M_table[index].erase(it);
					return ret;
				}
			}
		}
		return _M_values.end();
	}

	void print_list()
	{
		for(auto& value : _M_values)
		{
			std::cout << value.first << " " << value.second << std::endl;
		}
	}
	void print_table()
	{
		auto index = 0;
		for(auto& row : _M_table)
		{
			std::cout << index++ << ":";
			for(auto& value : row)
			{
				std::cout << " | " << value->first << "," << value->second;
			}
			std::cout << std::endl;
		}
	}

	private:
	size_t find_index(const KeyT& value, size_t size)
	{
		auto hash = _M_hash(value);
		return hash % size;	
	}

	list_type _M_values;
	bulk_type _M_table;
	HashFuncT _M_hash;
	size_t _M_count;
	size_t _M_size;

};

#endif 
