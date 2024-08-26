#ifndef __SMART_PTR__
#define __SMART_PTR__
#include <memory>

namespace memory
{

#include <atomic>

class RefCount2
{
  protected:

    void increment()
    {
        _M_count++;
    }

    bool increment_nz()
    {
        auto count = _M_count.load();
        while (count && !_M_count.compare_exchange_strong(count, count + 1))
        {
        }
        return count > 0 ? true : false;
    }

    std::uint64_t decrement()
    {
        return _M_count--;
    }

    void destroy_this()
    {
        delete this;
    }

    std::uint64_t count() const
    {
        return _M_count.load();
    }

  private:
    std::atomic_uint64_t _M_count = 0;
};

template<typename T>
class WrapPtr : public RefCount2
{
    using storage_type = T;

  public:

    bool reset(storage_type *ptr)
    {
        if (count() == 0)
        {
            _M_obj = ptr;
            increment();
            return true;
        }
        return false;
    }

    storage_type *try_acquire()
    {
        if (!increment_nz())
        {
            return nullptr;
        }
        return _M_obj;
    }

    void release()
    {
        if (decrement() == 1)
        {
            delete _M_obj;
        }
    }

    storage_type *_M_obj = nullptr;
};

} // namespace memory
#endif