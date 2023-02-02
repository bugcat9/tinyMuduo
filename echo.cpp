#include "echo.h"

#include "base/Logging.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

// using namespace tinyMuduo;
// using namespace tinyMuduo::net;

EchoServer::EchoServer(tinyMuduo::net::EventLoop *loop,
                       const tinyMuduo::net::InetAddress &listenAddr)
    : server_(loop, listenAddr, "EchoServer")
{
    server_.setConnectionCallback(
        std::bind(&EchoServer::onConnection, this, _1));
    server_.setMessageCallback(
        std::bind(&EchoServer::onMessage, this, _1, _2, _3));
}

void EchoServer::start()
{
    server_.start();
}

void EchoServer::onConnection(const tinyMuduo::net::TcpConnectionPtr &conn)
{
    LOG_INFO << "EchoServer - " << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");
}

void EchoServer::onMessage(const tinyMuduo::net::TcpConnectionPtr &conn,
                           tinyMuduo::net::Buffer *buf,
                           tinyMuduo::Timestamp time)
{
    tinyMuduo::string msg(buf->retrieveAllAsString());
    LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
             << "data received at " << time.toString();
    conn->send(msg);
}
