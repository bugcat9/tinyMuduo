set(base_SRCS
Timestamp.cpp
)

add_library(base ${base_SRCS})
target_link_libraries(base pthread rt)

add_subdirectory(test)