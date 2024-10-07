#include "base/Singleton.h"
#include "base/CurrentThread.h"
#include "base/Thread.h"

#include <stdio.h>
#include <boost/noncopyable.hpp>

class Test : boost::noncopyable
{
public:
    Test()
    {
        printf("tid=%d, constructing %p\n", tinyMuduo::CurrentThread::tid(), this);
    }

    ~Test()
    {
        printf("tid=%d, destructing %p %s\n", tinyMuduo::CurrentThread::tid(), this, name_.c_str());
    }

    const std::string &name() const { return name_; }
    void setName(const std::string &n) { name_ = n; }

private:
    std::string name_;
};

class TestNoDestroy : boost::noncopyable
{
public:
    // Tag member for Singleton<T>
    void no_destroy();
    TestNoDestroy()
    {
        printf("tid=%d, constructing TestNoDestroy %p\n", tinyMuduo::CurrentThread::tid(), this);
    }

    ~TestNoDestroy()
    {
        printf("tid=%d, destructing TestNoDestroy %p\n", tinyMuduo::CurrentThread::tid(), this);
    }
};

void threadFunc()
{
    printf("tid=%d, %p name=%s\n",
           tinyMuduo::CurrentThread::tid(),
           &tinyMuduo::Singleton<Test>::instance(),
           tinyMuduo::Singleton<Test>::instance().name().c_str());
    tinyMuduo::Singleton<Test>::instance().setName("only one, changed");
}

int main()
{
    tinyMuduo::Singleton<Test>::instance().setName("only one");
    tinyMuduo::Thread t1(threadFunc);
    t1.start();
    t1.join();
    printf("tid=%d, %p name=%s\n",
           tinyMuduo::CurrentThread::tid(),
           &tinyMuduo::Singleton<Test>::instance(),
           tinyMuduo::Singleton<Test>::instance().name().c_str());
    tinyMuduo::Singleton<TestNoDestroy>::instance();
    printf("with valgrind, you should see %zd-byte memory leak.\n", sizeof(TestNoDestroy));
}
