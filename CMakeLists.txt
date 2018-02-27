cmake_minimum_required(VERSION 3.9)
project(os_hw1)

set(CMAKE_CXX_STANDARD 11)


#add_executable(testBuffer
#        testBuffer.c
#        web_page_buffer.c web_page_buffer.h
#        web_thread.c web_thread.h)

add_executable(os_hw1
        client.c
        index.html
        Makefile
        nigel.jpg
        server.c
        server.h client.h)


