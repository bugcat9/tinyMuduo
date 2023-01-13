#ifndef _CONDITION_H
#define _CONDITION_H

#include "Mutex.h"
#include <condition_variable>
#include <boost/noncopyable.hpp>

namespace tinyMuduo
{
    class Condition : boost::noncopyable
    {
    private:
        MutexLock &mutex_;
        std::condition_variable cond_;

    public:
        explicit Condition(MutexLock &mutex)
            : mutex_(mutex)
        {
        }

        ~Condition()
        {
        }

        void wait()
        {
            MutexLock::UnassignGuard ug(mutex_);
            std::unique_lock<std::mutex> lk(*(mutex_.getPthreadMutex()));
            cond_.wait(lk);
        }

        // returns true if time out, false otherwise.
        bool waitForSeconds(int64_t seconds);

        void notify()
        {
            cond_.notify_one();
        }

        void notifyAll()
        {
            cond_.notify_all();
        }
    };
} // namespace tinyMudo

#endif // _CONDITION_H