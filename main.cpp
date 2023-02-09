#include "base/Logging.h"
#include "net/EventLoop.h"
#include "iostream"
#include "net/http/HttpServer.h"
#include "net/http/HttpRequest.h"
#include "net/http/HttpResponse.h"
#include <sys/stat.h>
#include <unistd.h>
#include <string>

using namespace tinyMuduo;
using namespace tinyMuduo::net;

std::map<string, string> user;

void onRequest(const HttpRequest &req, HttpResponse *resp)
{
  LOG_INFO << "Headers " << req.methodString() << " " << req.path();

  // char file[200];
  std::string file;
  if (!req.body().empty())
  {
    const char *bodyStr = req.body().c_str();
    // 将用户名和密码提取出来
    // user=123&passwd=123
    char name[100], password[100];
    int i;
    for (i = 5; bodyStr[i] != '&'; ++i)
      name[i - 5] = bodyStr[i];
    name[i - 5] = '\0';

    int j = 0;
    for (i = i + 10; bodyStr[i] != '\0'; ++i, ++j)
      password[j] = bodyStr[i];
    password[j] = '\0';

    if (req.path() == "/3CGISQL.cgi")
    {
      // 表示注册
      if (user.count(name))
      {
        // strcpy(file, "resources/registerError.html");
        file.append("resources/registerError.html");
      }
      else
      {
        // strcpy(file, "resources/login.html");
        file.append("resources/login.html");
      }
    }
    else if (req.path() == "/2CGISQL.cgi")
    {
      // 表示登录
      if (user.count(name) && user[name] == password)
      {

        // strcpy(file, "resources/welcome.html");
        file.append("resources/welcome.html");
      }
      else
      {
        // strcpy(file, "resources/logError.html");
        file.append("resources/logError.html");
      }
    }
  }
  else if (req.path() == "/0")
  {
    // strcpy(file, "resources/register.html");
    file.append("resources/register.html");
  }
  else if (req.path() == "/1")
  {
    // strcpy(file, "resources/login.html");
    file.append("resources/login.html");
  }
  else if (req.path() == "/5")
  {
    // strcpy(file, "resources/picture.html");
    file.append("resources/picture.html");
  }
  else if (req.path() == "/6")
  {
    // strcpy(file, "resources/video.html");
    file.append("resources/video.html");
  }
  else if (req.path() == "/7")
  {
    // strcpy(file, "resources/fans.html");
    file.append("resources/fans.html");
  }
  else
  {
    // strcpy(file, "resources");
    file.append("resources");
    // int len = strlen(file);
    const char *url_real = req.path().c_str();
    // strncpy(file + len, url_real, strlen(url_real) - 1);
    file.append(url_real);
    resp->setContentType("image/jpeg");
  }

  // 读取文件状态
  struct stat fileStat;
  if (stat(file.c_str(), &fileStat) < 0)
  {
    LOG_INFO << file << " no such file";
  }

  // if (!(fileStat.st_mode & S_IROTH))
  //   return;

  // if (S_ISDIR(fileStat.st_mode))
  //   return;

  resp->setStatusCode(HttpResponse::k200Ok);
  resp->setStatusMessage("OK");
  resp->addHeader("Server", "tinyMuduo");
  resp->setContentLength(fileStat.st_size);
  resp->setFile(file);
}

int main(int argc, char const *argv[])
{

  user.insert({"bugcat", "123456"});
  int numThreads = 5;
  EventLoop loop;
  HttpServer server(&loop, InetAddress(8080), "webserver");
  server.setHttpCallback(onRequest);
  server.setThreadNum(numThreads);
  server.start();
  loop.loop();
  return 0;
}
