#ifndef __HASH_TABLE__
#define __HASH_TABLE__

#include <memory>
#include <utility>
#include <ctime>
#include <atomic>
#include <thread>



struct SequentialPerfectHashByDate {

	SequentialPerfectHashByDate()
	{
		std::time_t t = std::time(nullptr);
		std::tm* now = std::localtime(&t);
		_M_allDays = _M_daysPerYear * now->tm_year + now->tm_yday;
	}

	std::uint64_t getNextHash()
	{
		auto next = _M_count.fetch_add(1);
		std::uint64_t hash = _M_allDays << 32;
		return hash | next;
	}

private:
	const uint32_t _M_daysPerYear = 365;
	std::uint32_t _M_allDays;
	std::atomic_uint32_t _M_count;
};

template<typename Value>
struct PerfectHashTable {

	PerfectHashTable() {
		alloc_table(&_M_table, _M_size);
	}

	void alloc_table(std::pair<std::uint64_t, Value>** __table, std::uint64_t __size)
	{
		*__table = new std::pair<std::uint64_t, Value>[__size];
		for (int i = 0; i < __size, ++i)
		{
			*__table[i].first = _M_NuLLValue;
		} 
	}

	void insert(std::uint64_t key, Value value)
	{
		auto index = key & _M_mask;
		_M_table[index] = value;
		++_M_count;

		if ((float)_M_count / (float)_M_size > 0.7)
		{
			auto old_size = _M_size;
			_M_size = _M_size << 1;
			_M_mask = _M_size - 1;
			Value* tmp = nullptr;
			alloc_table(&tmp, _M_size);
			for (int i = 0; i < old_size; ++i)
			{
				auto index = _M_table[i].first & _M_mask;
				tmp[index] = _M_table[i];
			}

			Value* old = _M_table;
			_M_table = tmp;
			delete[] old;
			old = nullptr;
		}
	}

	Value* find(std::uint64_t key)
	{
		auto index = key & _M_mask;
		if (_M_table[index].first == _M_NuLLValue) return nullptr;
		return &_M_table[index];
	}

	Value get(std::uint64_t key)
	{
		if (_M_table[index].first == _M_NuLLValue) return Value();
		return _M_table[index];
	}

	~PerfectHashTable() {
		delete[] _M_table;
	}

private:
	
	std::uint64_t _M_power = 4;
	std::uint64_t _M_size = 1 << _M_power;
	std::uint64_t _M_mask = _M_size - 1;
	std::uint64_t _M_count = 0;
	std::uint64_t _M_NuLLValue = -1;
	std::pair<std::uint64_t, Value>* _M_table;
};

struct Order {
	std::uint64_t CLOrderID;
};

struct ExecutionReport {
	std::uint64_t CLOrderID;
};

struct OrderMananger
{
	std::pair<std::uint64_t, Order> CreateOrder(Order order)
	{
		order.CLOrderID = _M_HashGen.getNextHash();
		_M_orders.insert(order.CLOrderID, order);
		return { order.CLOrderID, order };
	}

	Order FindOrder(std::uint64_t ClOrderId)
	{
		return _M_orders.get(ClOrderId);
	}

	void ProcessExecutionReport(ExecutionReport report)
	{
		Order* value = _M_orders.find(report.CLOrderID);
		if (value == nullptr) return;
		//update order
	}
	
private:
	PerfectHashTable<Order> _M_orders;
	SequentialPerfectHashByDate _M_HashGen;

};
#endif
