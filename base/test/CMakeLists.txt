add_executable(timestamp_unittest Timestamp_unittest.cpp)
target_link_libraries(timestamp_unittest base)
# add_test(NAME timestamp_unittest COMMAND timestamp_unittest)

add_executable(atomic_unittest Atomic_unittest.cpp)

add_executable(thread_test Thread_test.cpp)
target_link_libraries(thread_test base)

add_executable(exception_test Exception_test.cpp)
target_link_libraries(exception_test base)

add_executable(mutex_test Mutex_test.cpp)
target_link_libraries(mutex_test base)

# add_executable(mutex_test2 Mutex_test2.cpp)
# target_link_libraries(mutex_test2 base)

add_executable(blockingqueue_test BlockingQueue_test.cpp)
target_link_libraries(blockingqueue_test base)

add_executable(boundedblockingqueue_test BoundedBlockingQueue_test.cpp)
target_link_libraries(boundedblockingqueue_test base)

add_executable(singleton_test Singleton_test.cpp)
target_link_libraries(singleton_test base)

add_executable(threadlocal_test ThreadLocal_test.cpp)
target_link_libraries(threadlocal_test base)

add_executable(threadpool_test ThreadPool_test.cpp)
target_link_libraries(threadpool_test base)

add_executable(date_unittest Date_unittest.cpp)
target_link_libraries(date_unittest base)

add_executable(timezone_util TimeZone_util.cpp)
target_link_libraries(timezone_util base)

if(BOOSTTEST_LIBRARY)
add_executable(logstream_test LogStream_test.cpp)
target_link_libraries(logstream_test base boost_unit_test_framework)
add_test(NAME logstream_test COMMAND logstream_test)
endif()

add_executable(logging_test Logging_test.cpp)
target_link_libraries(logging_test base)


add_executable(fileutil_test FileUtil_test.cpp)
target_link_libraries(fileutil_test base)

add_executable(processinfo_test ProcessInfo_test.cpp)
target_link_libraries(processinfo_test base)

add_executable(logfile_test LogFile_test.cpp)
target_link_libraries(logfile_test base)