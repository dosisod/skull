#pragma once

#include <llvm-c/Core.h>

int setup_main(int, char *[]);

LLVMModuleRef generate_llvm(const char *, char *);

char *create_llvm_filename(const char *);
