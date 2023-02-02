set(net_SRCS
Acceptor.cpp
Buffer.cpp
Channel.cpp
Connector.cpp
EventLoop.cpp
EventLoopThread.cpp
EventLoopThreadPool.cpp
InetAddress.cpp
poller/Poller.cpp
poller/EPollPoller.cpp
Socket.cpp
SocketsOps.cpp
TcpClient.cpp
TcpConnection.cpp
TcpServer.cpp
Timer.cpp
TimerQueue.cpp
)

add_library(net ${net_SRCS})
target_link_libraries(net base)

add_subdirectory(test)