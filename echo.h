#ifndef _ECHO_H
#define _ECHO_H

#include "net/TcpServer.h"

// RFC 862
class EchoServer
{
public:
    EchoServer(tinyMuduo::net::EventLoop *loop,
               const tinyMuduo::net::InetAddress &listenAddr);

    void start(); // calls server_.start();

private:
    void onConnection(const tinyMuduo::net::TcpConnectionPtr &conn);

    void onMessage(const tinyMuduo::net::TcpConnectionPtr &conn,
                   tinyMuduo::net::Buffer *buf,
                   tinyMuduo::Timestamp time);

    tinyMuduo::net::TcpServer server_;
};

#endif // _ECHO_H
