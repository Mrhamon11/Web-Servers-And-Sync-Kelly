cmake_minimum_required(VERSION 3.9)
project(os_hw1)

set(CMAKE_CXX_STANDARD 11)

add_executable(testBuffer
        testBuffer.c
        buffer_struct.c
        buffer_struct.h)

add_executable(os_hw1
        client.c
        index.html
        Makefile
        nigel.jpg
        server.c
        buffer_struct.c
        buffer_struct.h)


