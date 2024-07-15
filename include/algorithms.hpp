#ifndef __ALGORITHMS__
#define __ALGORITHMS__

template<typename It>
static void swap(It rhs, It lhs)
{
	auto tmp = *rhs;
	*rhs = *lhs;
	*lhs = tmp;
};

template<typename RandomIt>
static RandomIt partition(RandomIt begin, RandomIt end)
{

	auto ret = begin;
	--end;
	auto pivot = *end;
	--end;
	for(; begin != end; ++begin)
	{
		if(*begin <=  pivot)
		{
//			swap<RandomIt>(ret, begin);
			++ret;
		}
	}
	++end;
//	swap<RandomIt>(ret, end); 
	return ret;
};

template<typename RandomIt>
static void quick_sort(RandomIt begin, RandomIt end)
{
	
	auto pivot = partition<RandomIt>(begin, end);
	quick_sort<RandomIt>((pivot + 1), end);
	quick_sort<RandomIt>(begin, (pivot - 1));
}
#endif
