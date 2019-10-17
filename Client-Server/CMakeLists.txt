cmake_minimum_required(VERSION 3.15)
project(queue C)

set(CMAKE_C_STANDARD 11)

add_executable(queue server.c client.c)