#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include <condition_variable>
#include <mutex>
#include <boost/noncopyable.hpp>
#include <deque>
#include <string>
#include <vector>
#include "Thread.h"

namespace tinyMuduo
{
    class ThreadPool : boost::noncopyable
    {

    public:
        typedef std::function<void()> Task;

        explicit ThreadPool(const std::string &nameArg = std::string("ThreadPool"));
        ~ThreadPool();

        // Must be called before start().
        void setMaxQueueSize(int maxSize) { maxQueueSize_ = maxSize; }
        void setThreadInitCallback(const Task &cb)
        {
            threadInitCallback_ = cb;
        }

        void start(int numThreads);
        void stop();

        const std::string &name() const
        {
            return name_;
        }

        size_t queueSize() const;

        // Could block if maxQueueSize > 0
        // Call after stop() will return immediately.
        // There is no move-only version of std::function in C++ as of C++14.
        // So we don't need to overload a const& and an && versions
        // as we do in (Bounded)BlockingQueue.
        // https://stackoverflow.com/a/25408989
        void run(Task f);

    private:
        bool isFull() const;
        void runInThread();
        Task take();

        mutable std::mutex mutex_;
        std::condition_variable notEmpty_;
        std::condition_variable notFull_;
        std::string name_;
        Task threadInitCallback_;
        std::vector<std::unique_ptr<tinyMuduo::Thread>> threads_;
        std::deque<Task> queue_;
        size_t maxQueueSize_;
        bool running_;
    };

} // namespace tinyMuduo

#endif // _THREADPOOL_H