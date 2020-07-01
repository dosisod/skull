#pragma once

#include <stdbool.h>
#include <uchar.h>

void c32sncpy(char32_t *, const char32_t *, size_t);
void c32slcpy(char32_t *, const char32_t *, size_t);
size_t c32slen(const char32_t *);
char32_t *c32sdup(const char32_t *);

char *c32stombs(const char32_t *);
char32_t *mbstoc32s(const char *);

bool c32scmp(const char32_t *, const char32_t *);
bool c32sncmp(const char32_t *, const char32_t *, size_t);

const char32_t *c32schr(const char32_t *, char32_t);
bool c32isdigit(char32_t);
bool c32isxdigit(char32_t);
bool c32isalnum(char32_t);
