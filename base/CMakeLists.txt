set(base_SRCS
Timestamp.cpp
CurrentThread.cpp
Exception.cpp
CountDownLatch.cpp
Thread.cpp
ThreadPool.cpp
Date.cpp
TimeZone.cpp
LogStream.cpp
Logging.cpp
FileUtil.cpp
ProcessInfo.cpp
LogFile.cpp
SqlConnectionPool.cpp
)

add_library(base ${base_SRCS})
target_link_libraries(base pthread rt mysqlclient)
#  ${CMAKE_SOURCE_DIR}设置为include路径，这里其实不合理，但是为了避免 #include"../../base"这种丑陋出现，目前好像只能这样
#  总体来说是项目目录不太规范导致的
target_include_directories(base PUBLIC ${CMAKE_SOURCE_DIR})
add_subdirectory(test)