#pragma once

#include <llvm-c/Core.h>

int setup_main(int, char *[]);
int build_file(char *);

void generate_llvm(const char *, const char *, char *);

char *create_llvm_filename(const char *);
char *create_llvm_main_func(const char *);
