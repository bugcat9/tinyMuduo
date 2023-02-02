#include "echo.h"

#include "base/Logging.h"
#include "net/EventLoop.h"

#include <unistd.h>

// using namespace tinyMuduo;
// using namespace tinyMuduo::net;

int main(int argc, char const *argv[])
{
  LOG_INFO << "pid = " << getpid();
  tinyMuduo::net::EventLoop loop;
  tinyMuduo::net::InetAddress listenAddr(2007);
  EchoServer server(&loop, listenAddr);
  server.start();
  loop.loop();
  return 0;
}
