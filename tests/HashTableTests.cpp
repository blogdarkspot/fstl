
#include <gtest/gtest.h>
#include "HashTable.hpp"
#include <string>

TEST(HashTableT, CreateHash)
{
	HashTable<std::string, int> ht;
	ht.insert("200", 10);
	ht.insert("002", 10);
	ht.insert("020", 10);
	ht.print_table();
	EXPECT_EQ(1,1);
}

