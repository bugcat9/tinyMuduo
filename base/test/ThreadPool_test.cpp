#include "base/ThreadPool.h"
#include "base/CountDownLatch.h"
#include "base/CurrentThread.h"
// #include "base/Logging.h"

#include <stdio.h>
#include <unistd.h> // usleep
#include <iostream>
void print()
{
    printf("tid=%d\n", tinyMuduo::CurrentThread::tid());
}

void printString(const std::string &str)
{
    // LOG_INFO << str;
    std::cout << str;
    usleep(100 * 1000);
}

void test(int maxSize)
{
    // LOG_WARN << "Test ThreadPool with max queue size = " << maxSize;
    printf("WARN Test ThreadPool with max queue size =  %d", maxSize);

    tinyMuduo::ThreadPool pool("MainThreadPool");
    pool.setMaxQueueSize(maxSize);
    pool.start(5);

    // LOG_WARN << "Adding";
    printf("WARN Adding\n");
    pool.run(print);
    pool.run(print);
    for (int i = 0; i < 100; ++i)
    {
        char buf[32];
        snprintf(buf, sizeof buf, "task %d\n", i);
        pool.run(std::bind(printString, std::string(buf)));
    }
    // LOG_WARN << "Done";
    printf("WARN Done\n");
    tinyMuduo::CountDownLatch latch(1);
    pool.run(std::bind(&tinyMuduo::CountDownLatch::countDown, &latch));
    latch.wait();
    pool.stop();
}

/*
 * Wish we could do this in the future.
void testMove()
{
  tinyMuduo::ThreadPool pool;
  pool.start(2);

  std::unique_ptr<int> x(new int(42));
  pool.run([y = std::move(x)]{ printf("%d: %d\n", tinyMuduo::CurrentThread::tid(), *y); });
  pool.stop();
}
*/

void longTask(int num)
{
    // LOG_INFO << "longTask " << num;
    tinyMuduo::CurrentThread::sleepUsec(3000000);
}

void test2()
{
    // LOG_WARN << "Test ThreadPool by stoping early.";
    tinyMuduo::ThreadPool pool("ThreadPool");
    pool.setMaxQueueSize(5);
    pool.start(3);

    tinyMuduo::Thread thread1([&pool]()
                              {
    for (int i = 0; i < 20; ++i)
    {
      pool.run(std::bind(longTask, i));
    } },
                              "thread1");
    thread1.start();

    tinyMuduo::CurrentThread::sleepUsec(5000000);
    // LOG_WARN << "stop pool";
    pool.stop(); // early stop

    thread1.join();
    // run() after stop()
    pool.run(print);
    // LOG_WARN << "test2 Done";
}

int main()
{
    // test(0);
    // test(1);
    test(5);
    // test(10);
    // test(50);
    // test2();
}
