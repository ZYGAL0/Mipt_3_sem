cmake_minimum_required(VERSION 3.15)
project(TCP C)

set(CMAKE_C_STANDARD 11)

add_executable(TCP
        #client.c
        server.c
        )