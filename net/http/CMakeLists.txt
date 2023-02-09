set(http_SRCS
  HttpServer.cpp
  HttpResponse.cpp
  HttpContext.cpp
  )

add_library(http ${http_SRCS})
target_link_libraries(http net)

add_executable(httpserver_test test/HttpServer_test.cpp)
target_link_libraries(httpserver_test http)

add_executable(httprequest_unittest test/HttpRequest_unittest.cpp)
target_link_libraries(httprequest_unittest http boost_unit_test_framework)