// TcpClient destructs in a different thread.

#include "../../base/Logging.h"
#include "../../net/EventLoopThread.h"
#include "../../net/TcpClient.h"

using namespace tinyMuduo;
using namespace tinyMuduo::net;

int main(int argc, char* argv[])
{
  Logger::setLogLevel(Logger::DEBUG);

  EventLoopThread loopThread;
  {
  InetAddress serverAddr("127.0.0.1", 1234); // should succeed
  TcpClient client(loopThread.startLoop(), serverAddr, "TcpClient");
  client.connect();
  CurrentThread::sleepUsec(500 * 1000);  // wait for connect
  client.disconnect();
  }

  CurrentThread::sleepUsec(1000 * 1000);
}
