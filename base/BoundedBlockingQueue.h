#ifndef _BOUNDEDBLOCKINGQUEUE_H
#define _BOUNDEDBLOCKINGQUEUE_H

#include <boost/circular_buffer.hpp>
#include <assert.h>
#include <boost/noncopyable.hpp>
#include <mutex>
#include <condition_variable>

namespace tinyMuduo
{
    template <typename T>
    class BoundedBlockingQueue : boost::noncopyable
    {
    public:
        explicit BoundedBlockingQueue(int maxSize)
            : mutex_(),
              notEmpty_(),
              notFull_(),
              queue_(maxSize)
        {
        }

        void put(const T &x)
        {
            std::unique_lock<std::mutex> lk(mutex_);

            while (queue_.full())
            {
                notFull_.wait(lk);
            }
            assert(!queue_.full());
            queue_.push_back(x);
            lk.unlock();
            notEmpty_.notify_one();
        }

        void put(T &&x)
        {
            std::unique_lock<std::mutex> lk(mutex_);

            while (queue_.full())
            {
                notFull_.wait(lk);
            }
            assert(!queue_.full());
            queue_.push_back(std::move(x));
            lk.unlock();
            notEmpty_.notify_one();
        }

        T take()
        {
            std::unique_lock<std::mutex> lk(mutex_);
            while (queue_.empty())
            {
                notEmpty_.wait(lk);
            }
            assert(!queue_.empty());
            T front(std::move(queue_.front()));
            queue_.pop_front();
            lk.unlock();
            notFull_.notify_one();
            return front;
        }

        bool empty() const
        {
            std::lock_guard<std::mutex> guard(mutex_);
            return queue_.empty();
        }

        bool full() const
        {
            std::lock_guard<std::mutex> guard(mutex_);
            return queue_.full();
        }

        size_t size() const
        {
            std::lock_guard<std::mutex> guard(mutex_);
            return queue_.size();
        }

        size_t capacity() const
        {
            std::lock_guard<std::mutex> guard(mutex_);
            return queue_.capacity();
        }

    private:
        mutable std::mutex mutex_;
        std::condition_variable notEmpty_;
        std::condition_variable notFull_;
        boost::circular_buffer<T> queue_;
    };
} // namespace tinyMuduo

#endif // _BOUNDEDBLOCKINGQUEUE_H