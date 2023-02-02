#ifndef _EVENTLOOPTHREAD_H
#define _EVENTLOOPTHREAD_H

#include "../base/Thread.h"

#include <mutex>
#include <condition_variable>


#include <boost/noncopyable.hpp>

namespace tinyMuduo
{
    namespace net
    {

        class EventLoop;

        class EventLoopThread : boost::noncopyable
        {
        public:
            typedef std::function<void(EventLoop *)> ThreadInitCallback;

            EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(),
                            const std::string &name = std::string());
            ~EventLoopThread();
            EventLoop *startLoop();

        private:
            void threadFunc();

            EventLoop *loop_;
            bool exiting_;
            Thread thread_;
            std::mutex mutex_;
            std::condition_variable cond_;
            ThreadInitCallback callback_;
        };

    } // namespace net
} // namespace tinyMuduo

#endif // _EVENTLOOPTHREAD_H