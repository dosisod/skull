add_executable(skull main.c)

target_link_libraries(skull)

target_compile_options(skull PRIVATE -Wall -Wextra -pedantic -Werror)