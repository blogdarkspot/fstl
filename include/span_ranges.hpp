#pragma once
#include <cassert>
#include <iterator>
#include <list>
#include <span>
#include <vector>

template <typename Ty> struct Ranges
{
    using ranges = std::vector<std::span<Ty>>;

    struct Iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using reference = Ty &;
        using pointer = Ty *;

        Iterator(ranges &r) : data(r), pos(0), index(0)
        {
        }

        reference operator*()
        {
            return data[index][pos];
        }

        pointer operator->()
        {
            return &(data[index][pos]);
        }
        Iterator &operator++()
        {
            if (pos >= data[index].size())
            {
                pos = 0x00;
                ++index;
            }
            else
            {
                ++pos;
            }
            return *this;
        }

        bool operator==(const Iterator &it) const
        {
            return it.pos == pos && it.index == index;
        }

        ranges &data;
        size_t pos;
        size_t index;
    };

    Ranges() = default;

    Ranges(ranges &list, std::size_t size) : m_ranges(list), m_size(size)
    {
    }
    Iterator begin()
    {
        return Iterator(m_ranges);
    }
    Iterator end()
    {
        auto ret = Iterator(m_ranges);
        ret.index = m_ranges.size() - 1;
        ret.pos = m_ranges[ret.index].size();
        return ret;
    }
    ranges get_range(std::size_t index, std::size_t count)
    {
        assert(index >= 0 && index < m_size);
        assert(count < m_size || index == 0 && count == m_size);
        ranges ret;
        auto it = m_ranges.begin();
        auto i = it->size();
        while (i < index)
        {
            i += it->size();
            ++it;
        }
        index = index - (i - it->size());
        while (count)
        {
            auto x = it->size() - index;
            auto tot = count > x ? x : count;
            count -= tot;
            auto sub = it->subspan(index, tot);
            ret.push_back(sub);
            index = 0;
            ++it;
        }
        return ret;
    }

    void remove_range(std::size_t index, std::size_t count)
    {
        assert(index >= 0 && index < m_size);
        assert(count < m_size || index == 0 && count == m_size);
        m_size -= count;
        auto it = m_ranges.begin();
        auto i = it->size();
        while (i < index)
        {
            ++it;
            i += it->size();
        }
        index = index - (i - it->size());
        if (index)
        {
            auto x = it->size() - index;
            auto tot = count > x ? x : count;
            auto sp = *it;
            auto sub = sp.subspan(0, index);
            *it = sub;
            if (count == tot)
            {
                if (index + count < sp.size())
                {
                    sub = sp.subspan(index + count);
                    it = m_ranges.insert(++it, sub);
                }
            }
            ++it;
            count -= tot;
        }
        while (count)
        {
            auto x = it->size();
            auto tot = count > x ? x : count;
            count -= tot;
            if (tot == it->size())
            {
                it = m_ranges.erase(it);
            }
            else
            {
                auto sub = it->subspan(tot);
                *it = sub;
                ++it;
            }
        }
    }

    void push_back(std::span<Ty> range)
    {
        m_size += range.size();
        m_ranges.push_back(range);
    }

    void push_back(ranges &r, size_t size)
    {
        m_size += size;
        m_ranges.insert(r.begin(), r.end(), m_ranges.end());
    }

    void push_front(ranges &r, size_t size)
    {
        m_size += size;
        m_ranges.insert(r.begin(), r.end(), m_ranges.begin());
    }

    void push_front(std::span<Ty> range)
    {
        m_size += range.size();
        m_ranges.insert(m_ranges.begin(), range);
    }

    std::size_t size() const
    {
        return m_size;
    }

    ranges m_ranges;
    std::size_t m_size = 0x00;
};
