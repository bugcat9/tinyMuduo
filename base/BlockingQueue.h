#ifndef _BLOCKINGQUEUE_H
#define _BLOCKINGQUEUE_H

// #include "Condition.h"
// #include "Mutex.h"
#include <mutex>
#include <condition_variable>

#include <deque>
#include <assert.h>
#include <boost/noncopyable.hpp>

namespace tinyMuduo
{
    template <typename T>
    class BlockingQueue : boost::noncopyable
    {

    public:
        using queue_type = std::deque<T>;
        BlockingQueue() : mutex_(), notEmpty_(), queue_()
        {
        }

        void put(const T &x)
        {
            std::lock_guard<std::mutex> guard(mutex_);
            queue_.push_back(x);
            notEmpty_.notify_one();
            // wait morphing saves us
            // http://www.domaigne.com/blog/computing/condvars-signal-with-mutex-locked-or-not/
        }

        void put(T &&x)
        {
            std::lock_guard<std::mutex> guard(mutex_);
            queue_.push_back(std::move(x));
            notEmpty_.notify_one();
        }

        T take()
        {
            std::unique_lock<std::mutex> lk(mutex_);
            // always use a while-loop, due to spurious wakeup
            while (queue_.empty())
            {
                notEmpty_.wait(lk);
            }
            assert(!queue_.empty());
            T front(std::move(queue_.front()));
            queue_.pop_front();
            lk.unlock();
            return front;
        }

        queue_type drain()
        {
            std::deque<T> queue;
            {
                std::lock_guard<std::mutex> guard(mutex_);
                queue = std::move(queue_);
                assert(queue_.empty());
            }
            return queue;
        }

        size_t size() const
        {
            std::lock_guard<std::mutex> guard(mutex_);
            return queue_.size();
        }

    private:
        mutable std::mutex mutex_;
        std::condition_variable notEmpty_;
        queue_type queue_;
    };

} // namespace tinyMuduo

#endif // _BLOCKINGQUEUE_H
