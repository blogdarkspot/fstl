#ifndef __SMART_PTR__
#define __SMART_PTR__

#include <atomic>

namespace memory
{

class RefCount
{
  public:
    void increment()
    {
        _M_count.fetch_add(1);
    }

    void decrement()
    {
        _M_count.fetch_sub(1);
    }

    std::uint64_t count()
    {
        return _M_count.load();
    }

  private:
    std::atomic_uint64_t _M_count = 1;
};

template <typename T, typename ReferencePolicy = RefCount> class StorageRefCount : public ReferencePolicy
{
  public:

    using value_type = T;
    using reference = T&;
    using pointer = T*;

    template<typename... Args>
    StorageRefCount(Args &&...vals) 
    {
        _M_pointer = new T(std::forward<Args>(vals)...);
    }

    reference operator*()
    {
        return *_M_pointer;
    }

    pointer operator->()
    {
        return _M_pointer;
    }

    StorageRefCount(const StorageRefCount<T, ReferencePolicy> &) = delete;
    StorageRefCount(StorageRefCount<T, ReferencePolicy> &) = delete;
    StorageRefCount(const StorageRefCount<T, ReferencePolicy> &&) = delete;
    StorageRefCount(StorageRefCount<T, ReferencePolicy> &&) = delete;
    StorageRefCount<T, ReferencePolicy> operator=(const StorageRefCount<T, ReferencePolicy> &) = delete;
    StorageRefCount<T, ReferencePolicy> operator=(StorageRefCount<T, ReferencePolicy> &) = delete;

    void release()
    {
        delete _M_pointer;
    }

  private:
    T* _M_pointer;
};

template<typename T>
class SmartPtr
{
  public:

      using value_type = typename StorageRefCount<T>::value_type;
      using reference = value_type&;
      using pointer = value_type*;

    template<typename... Params>
    SmartPtr(Params &&...args)
    {
        _M_count = new StorageRefCount<T>(std::forward<Params>(args)...);
    }

    SmartPtr(const SmartPtr<T> &other)
    {
        other._M_count->increment();
        _M_count = other._M_count;
    }

    reference operator*()
    {
        return _M_count->operator*();
    }

    pointer operator->()
    {
        return _M_count->operator->();
    }

    reference operator=(SmartPtr<T>& other)
    {
        other._M_count->increment();
        //replace
        if (_M_count)
        {
            decrement_and_release();
        }

        _M_count = other._M_count;
    }

    
    ~SmartPtr()
    {
        decrement_and_release(); 
    }

    void decrement_and_release()
    {
        _M_count->decrement();
        if (_M_count->count() == 0)
        {
            _M_count->release();
            delete _M_count;
        }
    }
  private:
    StorageRefCount<T> *_M_count;
};


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
        return (_M_count--) - 1;
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
    using value_type = T;

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

    storage_type* try_acquire()
    {
        
        /*
        if (!increment_nz())
        {
            return nullptr;
        }*/
        increment();
        return _M_obj;
    }


    void release()
    {
        if (decrement() == 0)
        {
            delete _M_obj;
        }
    }

    storage_type *_M_obj = nullptr;
};

template<typename T, template <class> typename OwnershipManager = WrapPtr> 
class weak_unique_ptr
{
  public:
    using value_type = typename OwnershipManager<T>::value_type;
    using pointer = value_type*;
    using reference = value_type&;

    weak_unique_ptr(OwnershipManager<T>* refCount) : _M_refCount(refCount)
    {
        _M_data = _M_refCount->try_acquire();
    }

    ~weak_unique_ptr()
    {
        if (_M_data)
        {
            _M_refCount->release();
        }
    }

    operator bool() const
    {
        return _M_data != nullptr; 
    }

    bool operator!() const
    {
        return _M_data == nullptr;
    }

    reference operator*()
    {
        return *_M_data;
    }

    const pointer operator->()
    {
        return _M_data;
    }

    weak_unique_ptr(const weak_unique_ptr &) = delete;
    weak_unique_ptr(weak_unique_ptr &) = delete;
    weak_unique_ptr(const weak_unique_ptr &&) = delete;
    weak_unique_ptr(weak_unique_ptr &&) = delete;

    weak_unique_ptr &operator=(const weak_unique_ptr &) = delete;
    weak_unique_ptr &operator=(weak_unique_ptr &) = delete;

    OwnershipManager<T>* _M_refCount;
    pointer _M_data = nullptr;
};

} // namespace memory
#endif