cmake_minimum_required(VERSION 3.19)
project(cs_projects)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

add_custom_target(cs_projects TEST COMMAND make assignment1 ${CMAKE_CURRENT_SOURCE_DIR})


#add_executable(cs_projects main.cpp)