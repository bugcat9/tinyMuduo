#include "../../base/CurrentThread.h"
#include "../../base/Thread.h"

#include <stdio.h>

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

thread_local Test testObj1;
thread_local Test testObj2;

void print()
{
    printf("tid=%d, obj1 %p name=%s\n",
           tinyMuduo::CurrentThread::tid(),
           &testObj1,
           testObj1.name().c_str());
    printf("tid=%d, obj2 %p name=%s\n",
           tinyMuduo::CurrentThread::tid(),
           &testObj2,
           testObj2.name().c_str());
}

void threadFunc()
{
    print();
    testObj1.setName("changed 1");
    testObj2.setName("changed 42");
    print();
}

int main()
{
    testObj1.setName("main one");
    print();
    tinyMuduo::Thread t1(threadFunc);
    t1.start();
    t1.join();
    testObj2.setName("main two");
    print();
    
}
