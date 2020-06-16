add_executable(Test main.c)

include_directories(../src/)

target_link_libraries(Test)

target_compile_options(Test PRIVATE ${EXTRA_COMPILER_FLAGS})
