#ifndef _TIMERID_H
#define _TIMERID_H

#include "../base/copyable.h"

#include <stdint.h>
namespace tinyMuduo
{
    namespace net
    {

        class Timer;

        ///
        /// An opaque identifier, for canceling Timer.
        ///
        class TimerId : public tinyMuduo::copyable
        {
        public:
            TimerId()
                : timer_(NULL),
                  sequence_(0)
            {
            }

            TimerId(Timer *timer, int64_t seq)
                : timer_(timer),
                  sequence_(seq)
            {
            }

            // default copy-ctor, dtor and assignment are okay

            friend class TimerQueue;

        private:
            Timer *timer_;
            int64_t sequence_;
        };

    } // namespace net
} // namespace tinyMuduo

#endif // _TIMERID_H