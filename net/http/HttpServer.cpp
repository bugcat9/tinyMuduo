
#include "../../net/http/HttpServer.h"

#include "../../base/Logging.h"
#include "../../net/http/HttpContext.h"
#include "../../net/http/HttpRequest.h"
#include "../../net/http/HttpResponse.h"
#include <sys/stat.h>

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
                       const string &user,
                       const string &passwd,
                       const string &databaseName,
                       int sqlNum,
                       TcpServer::Option option)
    : server_(loop, listenAddr, name, option), user_(user), passwd_(passwd), databaseName_(databaseName), sqlNum_(sqlNum)
{
    server_.setConnectionCallback(
        std::bind(&HttpServer::onConnection, this, _1));
    server_.setMessageCallback(
        std::bind(&HttpServer::onMessage, this, _1, _2, _3));
    server_.setWriteCompleteCallback(
        std::bind(&HttpServer::onWriteComplete, this, _1));

    this->setHttpCallback(std::bind(&HttpServer::onHttpProcess, this, _1, _2));
    // user_ = "root";
    // passwd_ = "123456";
    // databaseName_ = "yourdb";
    // sqlNum_ = 8;
}

/**
 * @brief 初始化数据库
 *
 * @param connPool
 */
void HttpServer::initmysql(ConnectionPool *connPool)
{
    // 先从连接池中取一个连接
    MYSQL *mysql = NULL;
    tinyMuduo::ConnectionRAII mysqlcon(&mysql, connPool);

    // 在user表中检索username，passwd数据，浏览器端输入
    if (mysql_query(mysql, "SELECT username,passwd FROM user"))
    {
        LOG_ERROR << "SELECT error: " << mysql_error(mysql);
    }

    // 从表中检索完整的结果集
    MYSQL_RES *result = mysql_store_result(mysql);

    // 返回结果集中的列数
    int num_fields = mysql_num_fields(result);

    // 返回所有字段结构的数组
    MYSQL_FIELD *fields = mysql_fetch_fields(result);

    // 从结果集中获取下一行，将对应的用户名和密码，存入map中
    while (MYSQL_ROW row = mysql_fetch_row(result))
    {
        string temp1(row[0]);
        string temp2(row[1]);
        users[temp1] = temp2;
    }
}

void HttpServer::start()
{
    LOG_WARN << "HttpServer[" << server_.name()
             << "] starts listening on " << server_.ipPort();
    server_.start();
    // 初始化数据库连接池
    connPool_ = ConnectionPool::getInstance();
    connPool_->init("127.0.0.1", user_, passwd_, databaseName_, 3306, sqlNum_);
    initmysql(connPool_);
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
    else
    {
        LOG_INFO << file << " no such file";
        conn->shutdown();
    }
}

void HttpServer::onWriteComplete(const TcpConnectionPtr &conn)
{

    char buf[TcpConnection::kBufSize];
    size_t nread = 0;
    if (conn->filePtr_)
        nread = ::fread(buf, 1, sizeof buf, get_pointer(conn->filePtr_));
    if (nread > 0)
    {
        conn->send(buf, static_cast<int>(nread));
    }
    else
    {
        conn->filePtr_.reset();
        LOG_INFO << "FileServer - done";
    }
}

/**
 * @brief 对http消息进行处理的函数
 *
 * @param req
 * @param resp
 */
void HttpServer::onHttpProcess(const HttpRequest &req, HttpResponse *resp)
{
    LOG_INFO << "Headers " << req.methodString() << " " << req.path();

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
            if (users.count(name))
            {
                file.append("resources/registerError.html");
            }
            else
            {
                // 如果是注册，先检测数据库中是否有重名的
                // 没有重名的，进行增加数据
                char *sql_insert = (char *)malloc(sizeof(char) * 200);
                strcpy(sql_insert, "INSERT INTO user(username, passwd) VALUES(");
                strcat(sql_insert, "'");
                strcat(sql_insert, name);
                strcat(sql_insert, "', '");
                strcat(sql_insert, password);
                strcat(sql_insert, "')");

                // 先从连接池中取一个连接
                MYSQL *mysql = NULL;
                ConnectionRAII mysqlcon(&mysql, connPool_);
                // 此处感觉需要锁一下
                int res = mysql_query(mysql, sql_insert);
                if (!res)
                {
                    users[name] = password;
                    file.append("resources/login.html");
                }
                else
                {
                    file.append("resources/registerError.html");
                }
            }
        }
        else if (req.path() == "/2CGISQL.cgi")
        {
            // 表示登录
            if (users.count(name) && users[name] == password)
            {

                file.append("resources/welcome.html");
            }
            else
            {
                file.append("resources/logError.html");
            }
        }
    }
    else if (req.path() == "/0")
    {
        file.append("resources/register.html");
    }
    else if (req.path() == "/1")
    {
        file.append("resources/login.html");
    }
    else if (req.path() == "/5")
    {
        file.append("resources/picture.html");
    }
    else if (req.path() == "/6")
    {
        file.append("resources/video.html");
    }
    else if (req.path() == "/7")
    {
        file.append("resources/fans.html");
    }
    else if (req.path() == "/404")
    {
        file.append("resources/404.html");
    }
    else
    {
        // strcpy(file, "resources");
        file.append("resources");
        // int len = strlen(file);
        const char *url_real = req.path().c_str();
        file.append(url_real);
    }

    // 读取文件状态
    struct stat fileStat;
    if (stat(file.c_str(), &fileStat) < 0)
    {
        LOG_INFO << file << " no such file";
        file.clear();
        file.append("resources/404.html");
        stat(file.c_str(), &fileStat);
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