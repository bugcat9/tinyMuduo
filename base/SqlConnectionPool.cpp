#include <mysql/mysql.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <list>
#include <pthread.h>
#include <iostream>
#include "SqlConnectionPool.h"

using namespace std;

namespace tinyMuduo
{
    ConnectionPool::ConnectionPool()
    {
        curConn_ = 0;
        freeConn_ = 0;
    }

    ConnectionPool *ConnectionPool::getInstance()
    {
        static ConnectionPool connPool;
        return &connPool;
    }

    // 构造初始化
    void ConnectionPool::init(string url, string User, string PassWord, string DBName, int Port, int MaxConn)
    {
        url_ = url;
        port_ = Port;
        user_ = User;
        passwd_ = PassWord;
        databaseName_ = DBName;

        for (int i = 0; i < MaxConn; i++)
        {
            MYSQL *con = NULL;
            con = mysql_init(con);

            if (con == NULL)
            {
                LOG_ERROR << "MySQL Error";
                exit(1);
            }
            con = mysql_real_connect(con, url.c_str(), User.c_str(), PassWord.c_str(), DBName.c_str(), Port, NULL, 0);

            if (con == NULL)
            {
                LOG_ERROR << "MySQL Error";
                exit(1);
            }
            connList.push_back(con);
            ++freeConn_;
        }

        maxConn_ = freeConn_;
    }

    // 当有请求时，从数据库连接池中返回一个可用连接，更新使用和空闲连接数
    MYSQL *ConnectionPool::getConnection()
    {
        MYSQL *con = NULL;

        if (0 == connList.size())
            return NULL;

        std::unique_lock<std::mutex> lk(mutex_);

        con = connList.front();
        connList.pop_front();

        --freeConn_;
        ++curConn_;
        while (freeConn_ <= 0)
        {
            /* code */
            condition_.wait(lk);
        }

        return con;
    }

    // 释放当前使用的连接
    bool ConnectionPool::releaseConnection(MYSQL *con)
    {
        if (NULL == con)
            return false;

        std::unique_lock<std::mutex> lk(mutex_);

        connList.push_back(con);
        ++freeConn_;
        --curConn_;

        // condition_.wait(lk);
        if (freeConn_ > 0)
        {
            condition_.notify_all();
        }
        return true;
    }

    // 销毁数据库连接池
    void ConnectionPool::destroyPool()
    {

        std::lock_guard<std::mutex> lock(mutex_);

        if (connList.size() > 0)
        {
            list<MYSQL *>::iterator it;
            for (it = connList.begin(); it != connList.end(); ++it)
            {
                MYSQL *con = *it;
                mysql_close(con);
            }
            curConn_ = 0;
            freeConn_ = 0;
            connList.clear();
        }
    }

    // 当前空闲的连接数
    int ConnectionPool::getFreeConn()
    {
        return this->freeConn_;
    }

    ConnectionPool::~ConnectionPool()
    {
        destroyPool();
    }

    ConnectionRAII::ConnectionRAII(MYSQL **SQL, ConnectionPool *connPool)
    {
        *SQL = connPool->getConnection();

        conRAII_ = *SQL;
        poolRAII_ = connPool;
    }

    ConnectionRAII::~ConnectionRAII()
    {
        poolRAII_->releaseConnection(conRAII_);
    }
} // namespace tinyMuduo
