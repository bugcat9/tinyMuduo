#ifndef _HTTPSERVER_H
#define _HTTPSERVER_H

#include "../../net/TcpServer.h"
#include "../../base/SqlConnectionPool.h"

namespace tinyMuduo
{
    namespace net
    {

        class HttpRequest;
        class HttpResponse;

        /// A simple embeddable HTTP server designed for report status of a program.
        /// It is not a fully HTTP 1.1 compliant server, but provides minimum features
        /// that can communicate with HttpClient and Web browser.
        /// It is synchronous, just like Java Servlet.
        class HttpServer : boost::noncopyable
        {
        public:
            typedef std::function<void(const HttpRequest &,
                                       HttpResponse *)>
                HttpCallback;
            HttpServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &name,
                       const string &user,
                       const string &passwd,
                       const string &databaseName,
                       int sqlNum,
                       TcpServer::Option option = TcpServer::kNoReusePort);

            EventLoop *getLoop() const { return server_.getLoop(); }

            /// Not thread safe, callback be registered before calling start().
            void setHttpCallback(const HttpCallback &cb)
            {
                httpCallback_ = cb;
            }

            void setThreadNum(int numThreads)
            {
                server_.setThreadNum(numThreads);
            }

            void start();

        private:
            void onConnection(const TcpConnectionPtr &conn);
            void onMessage(const TcpConnectionPtr &conn,
                           Buffer *buf,
                           Timestamp receiveTime);
            void onRequest(const TcpConnectionPtr &, const HttpRequest &);
            void onWriteComplete(const TcpConnectionPtr &conn);
            void initmysql(ConnectionPool *connPool);

            void onHttpProcess(const HttpRequest &req, HttpResponse *resp);
            TcpServer server_;
            HttpCallback httpCallback_;

            ConnectionPool *connPool_; // 数据库相关
            string user_;              // 登陆数据库用户名
            string passwd_;            // 登陆数据库密码
            string databaseName_;      // 使用数据库名
            int sqlNum_;
            map<string, string> users;
        };

    } // namespace net
} // namespace tinyMuduo

#endif // _HTTPSERVER_H