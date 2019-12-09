cmake_minimum_required(VERSION 3.6)
project(flood C)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -g")
set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES
                server.c)
        #        client.c)
#        client2.c)

add_executable(flood ${SOURCE_FILES})

target_link_libraries(flood pthread)

