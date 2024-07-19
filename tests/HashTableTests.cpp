
#include "gtest/gtest.h"

#include "hash_table_lock_free.hpp"
#include <string>

TEST(HashTableT, CreateHash)
{
	hash_table_lock_free<std::string, int>x;
	x.insert({"pipoca", 3});
}

