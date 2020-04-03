Cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

project(msdscript)

add_executable(
    msdscript
    cont.cpp
    expr.cpp
    main.cpp
    parse.cpp
    value.cpp
    env.cpp
    pointer.cpp
    Step.cpp
)