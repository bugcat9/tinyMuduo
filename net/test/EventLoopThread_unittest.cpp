#include "../../net/EventLoopThread.h"
#include "../../net/EventLoop.h"
#include "../../base/Thread.h"
#include "../../base/CountDownLatch.h"

#include <stdio.h>
#include <unistd.h>

using namespace tinyMuduo;
using namespace tinyMuduo::net;

void print(EventLoop *p = NULL)
{
    printf("print: pid = %d, tid = %d, loop = %p\n",
           getpid(), CurrentThread::tid(), p);
}

void quit(EventLoop *p)
{
    print(p);
    p->quit();
}

int main()
{
    print();

    {
        EventLoopThread thr1; // never start
    }

    {
        // dtor calls quit()
        EventLoopThread thr2;
        EventLoop *loop = thr2.startLoop();
        loop->runInLoop(std::bind(print, loop));
        CurrentThread::sleepUsec(500 * 1000);
    }

    {
        // quit() before dtor
        EventLoopThread thr3;
        EventLoop *loop = thr3.startLoop();
        loop->runInLoop(std::bind(quit, loop));
        CurrentThread::sleepUsec(500 * 1000);
    }
}
