#pragma once

#include <stdio.h>

char *file_to_string(FILE *const);
FILE *open_file(const char *, _Bool);
char *gen_filename(const char *, const char *);
_Bool is_directory(const char *);
