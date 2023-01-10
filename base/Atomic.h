#ifndef _ATOMIC_H
#define _ATOMIC_H

#include <boost/noncopyable.hpp>
#include <atomic>
#include <stdint.h>

namespace tinyMuduo
{
    /**
     * @brief 为了方便直接使用C++11 中的 atomic类进行封装
     *
     * @tparam T
     */
    template <typename T>
    class AtomicIntegerT : boost::noncopyable
    {
    public:
        AtomicIntegerT()
            : value_(0)
        {
        }
        T get()
        {
            return value_;
        }

        T getAndAdd(T x)
        {
            T old_value = value_.load();
            value_.store(old_value + x);
            return old_value;
        }

        T addAndGet(T x)
        {
            return getAndAdd(x) + x;
        }

        T incrementAndGet()
        {
            return addAndGet(1);
        }

        T decrementAndGet()
        {
            return addAndGet(-1);
        }

        void add(T x)
        {
            getAndAdd(x);
        }

        void increment()
        {
            incrementAndGet();
        }

        void decrement()
        {
            decrementAndGet();
        }

        T getAndSet(T newValue)
        {
            return value_.exchange(newValue);
        }

    private:
        std::atomic<T> value_;
    };

    typedef AtomicIntegerT<int32_t> AtomicInt32;
    typedef AtomicIntegerT<int64_t> AtomicInt64;
} // namespace tinyMuduo

#endif // _ATOMIC_H
