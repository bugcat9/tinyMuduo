// TcpClient destructs when TcpConnection is connected but unique.

#include "base/Logging.h"
#include "base/Thread.h"
#include "net/EventLoop.h"
#include "net/TcpClient.h"

using namespace tinyMuduo;
using namespace tinyMuduo::net;

void threadFunc(EventLoop *loop)
{
    InetAddress serverAddr("127.0.0.1", 1234); // should succeed
    TcpClient client(loop, serverAddr, "TcpClient");
    client.connect();

    CurrentThread::sleepUsec(1000 * 1000);
    // client destructs when connected.
}

int main(int argc, char *argv[])
{
    Logger::setLogLevel(Logger::DEBUG);

    EventLoop loop;
    loop.runAfter(3.0, std::bind(&EventLoop::quit, &loop));
    Thread thr(std::bind(threadFunc, &loop));
    thr.start();
    loop.loop();
}
