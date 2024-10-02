cmake_minimum_required(VERSION 3.25.1)
project(BackEnd)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(bsoncxx REQUIRED)
find_package(mongocxx REQUIRED)

add_executable(BackEnd main.cpp)

include_directories(./cpp-httplib/)
target_link_libraries(BackEnd PRIVATE bsoncxx_static mongocxx_static jsoncpp)
