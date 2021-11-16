cmake_minimum_required(VERSION 3.19)
project(cs_projects)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

add_custom_target(cs_projects TEST COMMAND make assignment1 ${CMAKE_CURRENT_SOURCE_DIR})

# -------------------------------------------------------

# enable conan support by conan cmake project
# Further Instructions and Documents refer : https://github.com/conan-io/cmake-conan

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
            "${CMAKE_BINARY_DIR}/conan.cmake"
            EXPECTED_HASH SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
            TLS_VERIFY ON)
endif()

#including dependencies headers
include(${CMAKE_BINARY_DIR}/conan.cmake)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# configure the frameworks support
# External framework or libs could be found on https://conan.io/center/

conan_cmake_run(REQUIRES

        dbg-macro/0.4.0
        gtest/1.11.0
        fmt/7.1.3

        BASIC_SETUP CMAKE_TARGETS
        BUILD missing)

conan_basic_setup()

add_subdirectory(assignment3)
add_subdirectory(workshop11)
add_subdirectory(assignment2)
add_subdirectory(workshop10)
#add_executable(cs_projects main.cpp)