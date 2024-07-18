#ifndef __ALGORITHMS__
#define __ALGORITHMS__
#include <memory>

template<typename It>
static void _swap(It rhs, It lhs)
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
	for(; begin != end; ++begin)
	{
		if(*begin <=  pivot)
		{
			_swap<RandomIt>(ret, begin);
			++ret;
		}
	}
	_swap<RandomIt>(ret, end); 
	return ret;
};

template<typename RandomIt>
static void quick_sort(RandomIt begin, RandomIt end)
{
	if(begin < end)
	{

		auto pivot = partition<RandomIt>(begin, end);
		quick_sort<RandomIt>((pivot + 1), end);
		quick_sort<RandomIt>(begin, (pivot - 1));
	}	
}


class heap_sort
{
	public:
	template<typename It>
	static void sort(It begin, It end)
	{
		heapfy(begin, end);
		while(begin != end)
		{
			--end;
			_swap(begin, end);
			max_heapfy(begin, end, 1);	
		}

	}

		template<typename It>
			static void heapfy(It begin, It end)
			{
				auto size = (end - begin) / 2;
				for(int i = size; i > 0; --i)
				{
					max_heapfy(begin, end, i);
				}
			}	

	private:

		template<typename It>
			static void max_heapfy(It begin, It end, size_t position)
			{
				auto right = get_right(position) - 1;
				auto left = get_left(position) - 1;
				auto current = --position;
				size_t size = end - begin;
				if(right < size  && begin[right] > begin[current])
				{
					current = right;	
				}
				if(left < size && begin[left] > begin[current])
				{
					current = left;
				}
				if(position != current)
				{
					_swap(begin + position , begin + current);
					max_heapfy(begin, end, current + 1);
				}
			}

		static size_t get_left(size_t pos)
		{
			return pos << 1;
		}

		static size_t  get_right(size_t pos)
		{
			return (pos << 1) + 1;
		}
};
#endif
