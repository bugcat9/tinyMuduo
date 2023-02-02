#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H

#include <atomic>
#include <functional>
#include <vector>
#include <memory>

#include <boost/any.hpp>
#include <boost/noncopyable.hpp>

#include "../base/Mutex.h"
#include "../base/CurrentThread.h"
#include "../base/Timestamp.h"
#include "Callbacks.h"
#include "TimerId.h"

namespace tinyMuduo
{
    namespace net
    {

        class Channel;
        class Poller;
        class TimerQueue;

        ///
        /// Reactor, at most one per thread.
        ///
        /// This is an interface class, so don't expose too much details.
        class EventLoop : boost::noncopyable
        {
        public:
            typedef std::function<void()> Functor;

            EventLoop();
            ~EventLoop(); // force out-line dtor, for std::unique_ptr members.

            ///
            /// Loops forever.
            ///
            /// Must be called in the same thread as creation of the object.
            ///
            void loop();

            /// Quits loop.
            ///
            /// This is not 100% thread safe, if you call through a raw pointer,
            /// better to call through shared_ptr<EventLoop> for 100% safety.
            void quit();

            ///
            /// Time when poll returns, usually means data arrival.
            ///
            Timestamp pollReturnTime() const { return pollReturnTime_; }

            int64_t iteration() const { return iteration_; }

            /// Runs callback immediately in the loop thread.
            /// It wakes up the loop, and run the cb.
            /// If in the same loop thread, cb is run within the function.
            /// Safe to call from other threads.
            void runInLoop(Functor cb);
            /// Queues callback in the loop thread.
            /// Runs after finish pooling.
            /// Safe to call from other threads.
            void queueInLoop(Functor cb);

            size_t queueSize() const;

            // timers

            ///
            /// Runs callback at 'time'.
            /// Safe to call from other threads.
            ///
            TimerId runAt(Timestamp time, TimerCallback cb);
            ///
            /// Runs callback after @c delay seconds.
            /// Safe to call from other threads.
            ///
            TimerId runAfter(double delay, TimerCallback cb);
            ///
            /// Runs callback every @c interval seconds.
            /// Safe to call from other threads.
            ///
            TimerId runEvery(double interval, TimerCallback cb);
            ///
            /// Cancels the timer.
            /// Safe to call from other threads.
            ///
            void cancel(TimerId timerId);

            // internal usage
            void wakeup();
            void updateChannel(Channel *channel);
            void removeChannel(Channel *channel);
            bool hasChannel(Channel *channel);

            // pid_t threadId() const { return threadId_; }
            void assertInLoopThread()
            {
                if (!isInLoopThread())
                {
                    abortNotInLoopThread();
                }
            }
            bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }
            // bool callingPendingFunctors() const { return callingPendingFunctors_; }
            bool eventHandling() const { return eventHandling_; }

            void setContext(const boost::any &context)
            {
                context_ = context;
            }

            const boost::any &getContext() const
            {
                return context_;
            }

            boost::any *getMutableContext()
            {
                return &context_;
            }

            static EventLoop *getEventLoopOfCurrentThread();

        private:
            void abortNotInLoopThread();
            void handleRead(); // waked up
            void doPendingFunctors();

            void printActiveChannels() const; // DEBUG

            typedef std::vector<Channel *> ChannelList;

            bool looping_; /* atomic */
            std::atomic<bool> quit_;
            bool eventHandling_;          /* atomic */
            bool callingPendingFunctors_; /* atomic */
            int64_t iteration_;
            const pid_t threadId_;
            Timestamp pollReturnTime_;
            std::unique_ptr<Poller> poller_;
            std::unique_ptr<TimerQueue> timerQueue_;
            int wakeupFd_;
            // unlike in TimerQueue, which is an internal class,
            // we don't expose Channel to client.
            std::unique_ptr<Channel> wakeupChannel_;
            boost::any context_;

            // scratch variables
            ChannelList activeChannels_;
            Channel *currentActiveChannel_;

            mutable MutexLock mutex_;
            std::vector<Functor> pendingFunctors_ ;
        };

    } // namespace net
} // namespace tinyMuduo

#endif // _EVENTLOOP_H