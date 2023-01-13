#ifndef _MUTEX_H
#define _MUTEX_H

#include "CurrentThread.h"
#include <boost/noncopyable.hpp>
#include <mutex>
#include <assert.h>

namespace tinyMuduo
{
    class MutexLock : boost::noncopyable
    {
    private:
        friend class Condition;
        std::mutex mutex_;
        pid_t holder_;

        void unassignHolder()
        {
            holder_ = 0;
        }

        void assignHolder()
        {
            holder_ = CurrentThread::tid();
        }

        class UnassignGuard : noncopyable
        {
        public:
            explicit UnassignGuard(MutexLock &owner)
                : owner_(owner)
            {
                owner_.unassignHolder();
            }

            ~UnassignGuard()
            {
                owner_.assignHolder();
            }

        private:
            MutexLock &owner_;
        };

    public:
        MutexLock() : holder_(0){};
        ~MutexLock(){};

        // must be called when locked, i.e. for assertion
        bool isLockedByThisThread() const
        {
            return holder_ == CurrentThread::tid();
        }

        void assertLocked() const
        {
            assert(isLockedByThisThread());
        }

        void lock()
        {
            mutex_.lock();
            assignHolder();
        }

        void unlock()
        {
            unassignHolder();
            mutex_.unlock();
        }

        std::mutex *getPthreadMutex() /* non-const */
        {
            return &mutex_;
        }
    };

    // Use as a stack variable, eg.
    // int Foo::size() const
    // {
    //   MutexLockGuard lock(mutex_);
    //   return data_.size();
    // }
    class MutexLockGuard : boost::noncopyable
    {
    private:
        /* data */
    public:
        explicit MutexLockGuard(MutexLock &mutex)
            : mutex_(mutex)
        {
            mutex_.lock();
        }

        ~MutexLockGuard()
        {
            mutex_.unlock();
        }

    private:
        MutexLock &mutex_;
    };

} // namespace tinyMuduo

// Prevent misuse like:
// MutexLockGuard(mutex_);
// A tempory object doesn't hold the lock for long!
#define MutexLockGuard(x) error "Missing guard object name"

#endif // _MUTEX_H