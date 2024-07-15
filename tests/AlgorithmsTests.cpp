#include "algorithms.hpp"
#include <gtest/gtest.h>

#include <vector>

TEST(sort, quick_sort)
{
	std::vector<int> test = {0, 1,2,3,4} ;
	quick_sort(test.begin(), test.end());		
};
