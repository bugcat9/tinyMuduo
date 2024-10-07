#include "base/SqlConnectionPool.h"
#include <map>
#include <iostream>

// 需要修改的数据库信息,登录名,密码,库名
string user = "root";
string passwd = "123456";
string databasename = "yourdb";

map<string, string> users;

void initmysqlResult(tinyMuduo::ConnectionPool *connPool)
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

int main(int argc, char const *argv[])
{
    // 初始化数据库连接池
    tinyMuduo::ConnectionPool *connPool = tinyMuduo::ConnectionPool::getInstance();
    connPool->init("127.0.0.1", user, passwd, databasename, 3306, 8);

    // 初始化数据库读取表
    initmysqlResult(connPool);
    for (auto iter = users.begin(); iter != users.end(); iter++)
    {
        cout << "username: " << iter->first << " passwd: " << iter->second << std::endl;
    }
    return 0;
}
