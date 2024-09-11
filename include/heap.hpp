#ifndef __HEAP__
#define __HEAP__

template<typename Iterator>
struct heap {

	
	static void Heapfy(Iterator begin, Iterator end)
	{
		auto init = (end - begin) / 2;
		for (; init > 0; --init)
		{
			maxHeapfy(begin, end, init);
		}
	}

private:

	static std::uint64_t left(std::uint64_t v)
	{
		return v << 1;
	}
	static std::uint64_t right(std::uint64_t v)
	{
		return (v << 1) + 1;
	}

	static void maxHeapfy(Iterator begin, Iterator end, std::uint64_t pos)
	{
		auto l = left(pos) - 1;
		auto r = right(pos) - 1;
		auto current = pos - 1;
		auto size = end - begin;
		if (l < size && *(begin + l) > *(begin + current))
		{
			current = l;
		}
		if (r < size && *(begin + r) > *(begin + current))
		{
			current = r;
		}
		if (current != pos - 1)
		{
			swap((begin + pos - 1), (begin + current));
			maxHeapfy(begin, end, current + 1);
		}
	}

	static void swap(Iterator& x, Iterator& y)
	{
		auto tmp = *x;
		*x = *y;
		*y = tmp;
	}


};
#endif