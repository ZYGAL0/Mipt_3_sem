cmake_minimum_required(VERSION 3.6)
project(thread_s C)

set(CMAKE_C_STANDARD 11)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -g")

#find_package(Threads)

add_executable(thread_s
        #        sem.c
        #        example_book.c
        #        robot.c
        #        robot_n.c
        Bakery_Algorithm.c
        )


target_link_libraries(thread_s pthread)