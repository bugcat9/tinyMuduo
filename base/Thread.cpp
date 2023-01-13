#include "Thread.h"
#include "CurrentThread.h"
#include "Exception.h"
#include "Timestamp.h"
#include <assert.h>
#include <sys/prctl.h>

namespace tinyMuduo
{
    void CurrentThread::cacheTid()
    {
        if (t_cachedTid == 0)
        {
            t_cachedTid = gettid();
            t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
        }
    }

    bool CurrentThread::isMainThread()
    {
        return tid() == ::getpid();
    }

    void CurrentThread::sleepUsec(int64_t usec)
    {
        struct timespec ts = {0, 0};
        ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
        ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000);
        ::nanosleep(&ts, NULL);
    }

    AtomicInt32 Thread::numCreated_;
    Thread::Thread(ThreadFunc func, const std::string &name) : started_(false),
                                                               joined_(false),
                                                               tid_(0),
                                                               func_(std::move(func)),
                                                               name_(name),
                                                               latch_(1)
    {
        setDefaultName();
    }

    void Thread::setDefaultName()
    {
        int num = numCreated_.incrementAndGet();
        if (name_.empty())
        {
            char buf[32];
            snprintf(buf, sizeof buf, "Thread%d", num);
            name_ = buf;
        }
    }

    void Thread::start()
    {
        assert(!started_);
        started_ = true;
        // ThreadData data = ThreadData(func_, name_, &tid_);
        m_thread_ = std::thread(std::bind(&Thread::runInThread, this));
        latch_.wait();
    }

    void Thread::join()
    {
        assert(started_);
        assert(!joined_);
        joined_ = true;
        m_thread_.join();
    }

    Thread::~Thread()
    {
        if (started_ && !joined_)
        {
            m_thread_.detach();
        }
    }

    void Thread::runInThread()
    {
        tid_ = CurrentThread::tid();
        latch_.countDown();
        CurrentThread::t_threadName = name_.empty() ? "tinyMuduoThread" : name_.c_str();
        ::prctl(PR_SET_NAME, CurrentThread::t_threadName);
        try
        {
            func_();
            CurrentThread::t_threadName = "finished";
        }
        catch (const Exception &ex)
        {
            CurrentThread::t_threadName = "crashed";
            fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
            fprintf(stderr, "reason: %s\n", ex.what());
            fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
            abort();
        }
        catch (const std::exception &ex)
        {
            CurrentThread::t_threadName = "crashed";
            fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
            fprintf(stderr, "reason: %s\n", ex.what());
            abort();
        }
        catch (...)
        {
            CurrentThread::t_threadName = "crashed";
            fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
            throw; // rethrow
        }
    }

} // namespace tinyMuduo
