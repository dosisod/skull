cmake_minimum_required(VERSION 3.10.2)

project(skull VERSION 0.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED true)

add_subdirectory(src)
add_subdirectory(test)
enable_testing()
add_test(NAME TestBuild COMMAND Test)