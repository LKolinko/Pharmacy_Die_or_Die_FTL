cmake_minimum_required(VERSION 3.25.1)
project(GenModul)

set(CMAKE_CXX_STANDARD 23)

add_executable(GenModul main.cpp)

include_directories(./cpp-httplib/)
target_link_libraries(GenModul jsoncpp)