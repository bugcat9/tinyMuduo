#include "EventLoopThread.h"

#include "EventLoop.h"

using namespace tinyMuduo;
using namespace tinyMuduo::net;

EventLoopThread::EventLoopThread(const ThreadInitCallback &cb,
                                 const std::string &name)
    : loop_(NULL),
      exiting_(false),
      thread_(std::bind(&EventLoopThread::threadFunc, this), name),
      mutex_(),
      cond_(),
      callback_(cb)
{
}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    if (loop_ != NULL) // not 100% race-free, eg. threadFunc could be running callback_.
    {
        // still a tiny chance to call destructed object, if threadFunc exits just now.
        // but when EventLoopThread destructs, usually programming is exiting anyway.
        loop_->quit();
        thread_.join();
    }
}

EventLoop *EventLoopThread::startLoop()
{
    assert(!thread_.started());
    thread_.start();

    EventLoop *loop = NULL;
    {
        std::unique_lock<std::mutex> lk(mutex_);

        while (loop_ == NULL)
        {
            cond_.wait(lk);
        }
        loop = loop_;
    }

    return loop;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;

    if (callback_)
    {
        callback_(&loop);
    }

    {
        std::unique_lock<std::mutex> lk(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }

    loop.loop();
    // assert(exiting_);
    std::lock_guard<std::mutex> lock(mutex_);
    loop_ = NULL;
}
