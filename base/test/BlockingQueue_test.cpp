#include "base/BlockingQueue.h"
#include "base/CountDownLatch.h"
#include "base/Thread.h"
#include "base/CurrentThread.h"

#include <memory>
#include <string>
#include <vector>
#include <stdio.h>
#include <unistd.h>

class Test
{
public:
    Test(int numThreads)
        : latch_(numThreads)
    {
        for (int i = 0; i < numThreads; ++i)
        {
            char name[32];
            snprintf(name, sizeof name, "work thread %d", i);
            threads_.emplace_back(new tinyMuduo::Thread(
                std::bind(&Test::threadFunc, this), std::string(name)));
        }
        for (auto &thr : threads_)
        {
            thr->start();
        }
    }

    void run(int times)
    {
        printf("waiting for count down latch\n");
        latch_.wait();
        printf("all threads started\n");
        for (int i = 0; i < times; ++i)
        {
            char buf[32];
            snprintf(buf, sizeof buf, "hello %d", i);
            queue_.put(buf);
            printf("tid=%d, put data = %s, size = %zd\n", tinyMuduo::CurrentThread::tid(), buf, queue_.size());
        }
    }

    void joinAll()
    {
        for (size_t i = 0; i < threads_.size(); ++i)
        {
            queue_.put("stop");
        }

        for (auto &thr : threads_)
        {
            thr->join();
        }
    }

private:
    void threadFunc()
    {
        printf("tid=%d, %s started\n",
               tinyMuduo::CurrentThread::tid(),
               tinyMuduo::CurrentThread::name());

        latch_.countDown();
        bool running = true;
        while (running)
        {
            std::string d(queue_.take());
            printf("tid=%d, get data = %s, size = %zd\n", tinyMuduo::CurrentThread::tid(), d.c_str(), queue_.size());
            running = (d != "stop");
        }

        printf("tid=%d, %s stopped\n",
               tinyMuduo::CurrentThread::tid(),
               tinyMuduo::CurrentThread::name());
    }

    tinyMuduo::BlockingQueue<std::string> queue_;
    tinyMuduo::CountDownLatch latch_;
    std::vector<std::unique_ptr<tinyMuduo::Thread>> threads_;
};

void testMove()
{
    tinyMuduo::BlockingQueue<std::unique_ptr<int>> queue;
    queue.put(std::unique_ptr<int>(new int(42)));
    std::unique_ptr<int> x = queue.take();
    printf("took %d\n", *x);
    *x = 123;
    queue.put(std::move(x));
    std::unique_ptr<int> y = queue.take();
    printf("took %d\n", *y);
}

int main()
{
    printf("pid=%d, tid=%d\n", ::getpid(), tinyMuduo::CurrentThread::tid());
    Test t(5);
    t.run(100);
    t.joinAll();

    testMove();

    printf("number of created threads %d\n", tinyMuduo::Thread::numCreated());
}
