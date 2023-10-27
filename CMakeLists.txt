cmake_minimum_required(VERSION 3.20.0)
project(raytracing VERSION 0.1.0 LANGUAGES CXX)


set ( CMAKE_CXX_STANDARD          23 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )
set ( CMAKE_CXX_EXTENSIONS        OFF )

include(CTest)
enable_testing()

add_executable(raytracing main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
