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
)

add_library(base ${base_SRCS})
target_link_libraries(base pthread rt)

add_subdirectory(test)