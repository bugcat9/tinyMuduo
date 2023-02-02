cmake_minimum_required(VERSION 3.10)

project(tinyMuduo)

set(CXX_FLAGS -g -Wall)
set(CMAKE_CXX_COMPILER "g++")
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

find_package(Boost REQUIRED)
find_library(BOOSTTEST_LIBRARY NAMES boost_unit_test_framework)

add_subdirectory(base)
add_subdirectory(net)

add_executable(main main.cpp echo.cpp) 
target_link_libraries(main base net)