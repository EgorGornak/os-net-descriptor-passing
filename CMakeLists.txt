cmake_minimum_required(VERSION 3.10)
project(os-net--descriptor-passing)

set(CMAKE_CXX_STANDARD 14)

add_executable(server server.cpp)
add_executable(client client.cpp)