#ifndef _COUNTDOWNLATCH_H
#define _COUNTDOWNLATCH_H

#include <boost/noncopyable.hpp>
#include <mutex>
#include <condition_variable>

namespace tinyMuduo
{
    class CountDownLatch : boost::noncopyable
    {

    public:
        explicit CountDownLatch(int count);

        void wait();

        void countDown();

        int getCount() const;

    private:
        mutable std::mutex mutex_;
        std::condition_variable condition_;
        int count_;
    };

} // namespace tinyMuduo

#endif // _COUNTDOWNLATCH_H