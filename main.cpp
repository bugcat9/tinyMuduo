#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <string.h>

#include <vector>
#include <algorithm>
#include <iostream>

// 重定义epoll_event 数组
typedef std::vector<epoll_event> EventList;
#define ERR_EXIT(m)     \
  do                    \
  {                     \
    perror(m);          \
    exit(EXIT_FAILURE); \
  } while (0)

#define PORT 8080

int main()
{

  /***
   * 屏蔽 SIGPIPE SIGCHLD 信号
   */
  signal(SIGPIPE, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);

  /***
   * 打开一个文件描述符，解决EMFILE错误
   */
  int idlefd = open("/dev/null", O_RDONLY | O_CLOEXEC);

  int listenfd;
  if ((listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP)) < 0)
    ERR_EXIT("socket");

  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  /***
   * 设置端口重用，无需 TIME_WAIT
   */
  int on = 1;
  if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    ERR_EXIT("setsockopt");

  if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    ERR_EXIT("bind");
  if (listen(listenfd, SOMAXCONN) < 0)
    ERR_EXIT("listen");

  std::vector<int> clients;
  int epollfd;
  epollfd = epoll_create1(EPOLL_CLOEXEC);

  struct epoll_event event;
  event.data.fd = listenfd;
  event.events = EPOLLIN /* | EPOLLET*/;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event);

  EventList events(16);
  struct sockaddr_in peeraddr;
  socklen_t peerlen;
  int connfd;

  int nready;
  while (1)
  {
    nready = epoll_wait(epollfd, &*events.begin(), static_cast<int>(events.size()), -1);
    if (nready == -1)
    {
      if (errno == EINTR)
        continue;

      ERR_EXIT("epoll_wait");
    }
    if (nready == 0) // 什么都没有发生
      continue;

    /***
     * 当nready达到events的大小，说明可能有更多的事件触发
     */
    if ((size_t)nready == events.size())
      events.resize(events.size() * 2);

    for (int i = 0; i < nready; ++i)
    {
      if (events[i].data.fd == listenfd)
      {
        peerlen = sizeof(peeraddr);
        connfd = accept4(listenfd, (struct sockaddr *)&peeraddr,
                         &peerlen, SOCK_NONBLOCK | SOCK_CLOEXEC);

        if (connfd == -1)
        {
          if (errno == EMFILE)
          {
            /***
             * 发生EMFILE时的处理方法
             */

            close(idlefd);
            idlefd = accept(listenfd, NULL, NULL);
            close(idlefd);
            idlefd = open("/dev/null", O_RDONLY | O_CLOEXEC);
            continue;
          }
          else
            ERR_EXIT("accept4");
        }

        std::cout << "ip=" << inet_ntoa(peeraddr.sin_addr) << " port=" << ntohs(peeraddr.sin_port) << std::endl;

        clients.push_back(connfd);

        event.data.fd = connfd;
        event.events = EPOLLIN /* | EPOLLET*/;
        epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &event);
      }
      else if (events[i].events & EPOLLIN)
      {
        connfd = events[i].data.fd;
        if (connfd < 0)
          continue;

        char buf[1024] = {0};
        int ret = read(connfd, buf, 1024);
        if (ret == -1)
          ERR_EXIT("read");
        if (ret == 0)
        {
          /***
           * 说明对方断开了连接
          */
          std::cout << "client close" << std::endl;
          close(connfd);
          event = events[i];
          epoll_ctl(epollfd, EPOLL_CTL_DEL, connfd, &event);
          clients.erase(std::remove(clients.begin(), clients.end(), connfd), clients.end());
          continue;
        }

        std::cout << buf;
        /***
         * 写回对应的 内容，成为echo服务器
        */
        write(connfd, buf, strlen(buf));
      }
    }
  }

  return 0;
}
