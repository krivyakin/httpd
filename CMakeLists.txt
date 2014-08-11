cmake_minimum_required(VERSION 2.6)
set(PROJECT "httpd")

set(CMAKE_CXX_FLAGS "-std=c++11 -Werror -Wall -Wextra -pthread -g")

aux_source_directory(${CMAKE_SOURCE_DIR}/src/ SRCS)
file(GLOB HDRS "${CMAKE_SOURCE_DIR}/src/*.h")

add_executable(${PROJECT} ${SRCS})
add_dependencies(${PROJECT} ${HDRS})
target_link_libraries(${PROJECT} pthread)
