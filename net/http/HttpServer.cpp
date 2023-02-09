
#include "../../net/http/HttpServer.h"

#include "../../base/Logging.h"
#include "../../net/http/HttpContext.h"
#include "../../net/http/HttpRequest.h"
#include "../../net/http/HttpResponse.h"

using namespace tinyMuduo;
using namespace tinyMuduo::net;

namespace tinyMuduo
{
    namespace net
    {
        namespace detail
        {

            void defaultHttpCallback(const HttpRequest &, HttpResponse *resp)
            {
                resp->setStatusCode(HttpResponse::k404NotFound);
                resp->setStatusMessage("Not Found");
                resp->setCloseConnection(true);
            }

        } // namespace detail
    }     // namespace net
} // namespace tinyMuduo

HttpServer::HttpServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &name,
                       TcpServer::Option option)
    : server_(loop, listenAddr, name, option),
      httpCallback_(detail::defaultHttpCallback)
{
    server_.setConnectionCallback(
        std::bind(&HttpServer::onConnection, this, _1));
    server_.setMessageCallback(
        std::bind(&HttpServer::onMessage, this, _1, _2, _3));
    server_.setWriteCompleteCallback(
        std::bind(&HttpServer::onWriteComplete, this, _1));
}

void HttpServer::start()
{
    LOG_WARN << "HttpServer[" << server_.name()
             << "] starts listening on " << server_.ipPort();
    server_.start();
}

void HttpServer::onConnection(const TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        conn->setContext(HttpContext());
    }
}

void HttpServer::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buf,
                           Timestamp receiveTime)
{
    HttpContext *context = boost::any_cast<HttpContext>(conn->getMutableContext());
    // LOG_INFO << buf->toStringPiece();
    if (!context->parseRequest(buf, receiveTime))
    {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if (context->gotAll())
    {
        onRequest(conn, context->request());
        context->reset();
    }
}

void HttpServer::onRequest(const TcpConnectionPtr &conn, const HttpRequest &req)
{
    const string &connection = req.getHeader("Connection");
    bool close = connection == "close" ||
                 (req.getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
    HttpResponse response(close);
    httpCallback_(req, &response);
    Buffer buf;
    response.appendToBuffer(&buf);
    conn->send(&buf);
    const char *file = response.g_file.c_str();
    FILE *fp = ::fopen(file, "rb");
    if (fp)
    {
        TcpConnection::FilePtr ctx(fp, ::fclose);
        // conn->setContext(ctx);
        conn->filePtr_.swap(ctx);
        char buf[TcpConnection::kBufSize];
        size_t nread = ::fread(buf, 1, sizeof buf, fp);
        conn->send(buf, static_cast<int>(nread));
    }
}

void HttpServer::onWriteComplete(const TcpConnectionPtr &conn)
{

    char buf[TcpConnection::kBufSize];
    size_t nread = ::fread(buf, 1, sizeof buf, get_pointer(conn->filePtr_));
    if (nread > 0)
    {
        conn->send(buf, static_cast<int>(nread));
    }
    else
    {
        // conn->shutdown();
        LOG_INFO << "FileServer - done";
    }
}