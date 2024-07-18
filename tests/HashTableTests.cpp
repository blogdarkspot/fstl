
#include <gtest/gtest.h>
#include "hash_table.hpp"
#include "hash_table_lock_free.hpp"
#include <string>

TEST(HashTableT, CreateHash)
{
	auto v = xhash_table<std::string, int>();
	v.insert({"pipoca", 1});
	v.insert({"arroz", 2});
	v.insert({"cachorro", 4});
	v.print_table();
	v.print_list();
	v.erase("cachorro");
	v.print_table();
	v.print_list();

	auto x = hash_table_lock_free<std::string, int>();
	x.insert({"pipoca", 3});
}

