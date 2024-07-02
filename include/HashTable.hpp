#ifndef __HASH_EXP__
#define __HASH_EXP__
#include <iostream>
#include <memory>

std::uint64_t Bit64_SET = 0xFFFFFFFFFFFFFF;
std::uint64_t Bit32_SET = 0x000000FFFFFFFF;
std::uint64_t Bit16_SET = 0x0000000000FFFF;
std::uint64_t Bit8_SET  = 0x000000000000FF;
std::uint64_t Bit4_SET  = 0x0000000000000F;
std::uint64_t Bit2_SET  = 0x00000000000003;
std::uint64_t Bit1_SET  = 0x00000000000001;

template<typename Key, typename Value, typename Hasher = std::hash<Key>>
struct HashTable {

	using size_type = std::size_t;

	size_type nextPowerOfTwo(size_type value)
	{
		std::uint64_t ret = 0x01 << 63;
		while (ret & value)
		{
			ret >> 1;
		}
		return ret;
	}

		
	struct pair {
		Key _M_key;
		Value _M_value;
	};

	struct node {
		pair _M_value;
		node* _M_next;
	};

	HashTable() {
		_M_size = 1 << 3;
		_M_table = new node* [_M_size];
		_M_end = new node();
		for (auto i = 0; i < _M_size; ++i)
		{
			_M_table[i] = _M_end;
		}
	}

	void insert(Key key, Value value) {
		auto index = hash(key);
		node* new_node = new node();
		new_node->_M_value._M_key = key;
		new_node->_M_value._M_value = value;
		new_node->_M_next = _M_end;
		node* pos = _M_table[index];
		if (pos == _M_end)
		{
			_M_table[index] = new_node;
		}
		else
		{
			for (; pos != _M_end;)
			{
				if (pos->_M_value._M_key == key)
				{
					pos = _M_end;
				}
				else
				{
					if (pos->_M_next == _M_end) break;
				}
				pos = pos->_M_next;
			}
			if (pos == _M_end) return;

			pos->_M_next = new_node;
		}
		
	}

	std::size_t hash(Key key)
	{
		return Hasher{}(key) % _M_size;
	}

	~HashTable() {
		for (auto i = 0; i < _M_size; ++i)
		{
			if (_M_table[i] != _M_end)
			{
				node* pos = _M_table[i];
				while (pos != _M_end)
				{
					node* next = pos->_M_next;
					delete pos;
					pos = next;
				}
			}
		}
		delete _M_end;
		delete[] _M_table;
	}

	void print_table()
	{
		for (auto i = 0; i < _M_size; ++i)
		{
			if (_M_table[i] == _M_end)
			{
				std::cout << "position: " << i << " --- nullptr\n";
			}
			else
			{
				auto* pos = _M_table[i];
				std::cout << "position: " << i << " --- ";
				while (pos != _M_end)
				{
					std::cout << pos->_M_value._M_key << " | ";

					pos = pos->_M_next;
				}
				std::cout << std::endl;
				
			}
		}
	}

	node** _M_table;
	node* _M_end;
	std::uint64_t _M_size;
	std::uint64_t _M_count;
};
#endif