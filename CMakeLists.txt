cmake_minimum_required(VERSION 3.10.2)

project(skull VERSION 0.0)

add_subdirectory(src)
add_subdirectory(test)
enable_testing()
add_test(NAME TestBuild COMMAND Test)