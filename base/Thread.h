#ifndef _THREAD_H
#define _THREAD_H

#include <boost/noncopyable.hpp>
#include <functional>
#include <thread>
#include <string>
#include "Atomic.h"
#include "CountDownLatch.h"

namespace tinyMuduo
{
    class Thread : boost::noncopyable
    {

    public:
        typedef std::function<void()> ThreadFunc;
        Thread(ThreadFunc func, const std::string &name = std::string());
        ~Thread();

        void start();
        void join();

        bool started() const { return started_; }
        pid_t tid() const { return tid_; }
        const std::string &name() const { return name_; }

        static int numCreated() { return numCreated_.get(); }

    private:
        void setDefaultName();
        void runInThread();
        bool started_;
        bool joined_;
        pid_t tid_;
        std::thread m_thread_;
        ThreadFunc func_;
        std::string name_;
        CountDownLatch latch_;

        static AtomicInt32 numCreated_;
    };

} // namespace tinyMuduo

#endif // _THREAD_H