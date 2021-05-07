#pragma once

#include <stdbool.h>

bool generate_llvm(const char *, char *);
char *create_llvm_main_func(const char *);
bool write_file(char *);
