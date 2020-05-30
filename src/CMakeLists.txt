add_executable(skull main.c)

target_link_libraries(skull -ldl)

target_compile_options(skull PRIVATE ${EXTRA_COMPILER_FLAGS})