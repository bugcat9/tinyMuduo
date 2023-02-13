#include "base/Logging.h"
#include "net/EventLoop.h"
#include "net/http/HttpServer.h"
#include "net/http/HttpRequest.h"
#include "net/http/HttpResponse.h"

using namespace tinyMuduo;
using namespace tinyMuduo::net;

int main(int argc, char const *argv[])
{

  // 需要修改的数据库信息,登录名,密码,库名
  string user = "root";
  string passwd = "123456";
  string databasename = "yourdb";
  int sqlNum = 8;
  int numThreads = 5;

  EventLoop loop;
  HttpServer server(&loop, InetAddress(8080), "webserver", user, passwd, databasename, sqlNum);
  server.setThreadNum(numThreads);
  server.start();
  loop.loop();

  return 0;
}
