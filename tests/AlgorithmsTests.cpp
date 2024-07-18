#include "algorithms.hpp"
#include <gtest/gtest.h>

#include <vector>

TEST(sort, quick_sort)
{
	std::vector<int> result = {0,1,2,3,4};
	std::vector<int> test = {3,4,2,0,1};
	quick_sort(test.begin(), test.end());		
	
	for(int i = 0; i < test.size(); ++i)
	{
		EXPECT_EQ(result[i], test[i]);
	}
};

TEST(sort, heap_sort)
{
	std::vector<int> result = {0,1,2,3,4};
	std::vector<int> test = {3,4,2,0,1};
	heap_sort::sort(test.begin(), test.end());		
	
	for(int i = 0; i < test.size(); ++i)
	{
		EXPECT_EQ(result[i], test[i]);
	}
};
