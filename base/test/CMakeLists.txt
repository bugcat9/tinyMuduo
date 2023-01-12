add_executable(timestamp_unittest Timestamp_unittest.cpp)
target_link_libraries(timestamp_unittest base)
# add_test(NAME timestamp_unittest COMMAND timestamp_unittest)

add_executable(atomic_unittest Atomic_unittest.cpp)

add_executable(thread_test Thread_test.cpp)
target_link_libraries(thread_test base)

add_executable(exception_test Exception_test.cpp)
target_link_libraries(exception_test base)