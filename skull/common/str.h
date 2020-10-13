#pragma once

#include <uchar.h>

const char *strrstr(const char *, const char *);

void c32sncpy(char32_t *, const char32_t *, size_t);
size_t c32slen(const char32_t *);
char32_t *c32sdup(const char32_t *const);
char32_t *c32scat(const char32_t *const, const char32_t *const);

char *c32stombs(const char32_t *);
char32_t *mbstoc32s(const char *);

_Bool c32scmp(const char32_t *, const char32_t *);
_Bool c32sncmp(const char32_t *, const char32_t *, size_t);

const char32_t *c32schr(const char32_t *, char32_t);
_Bool c32isdigit(char32_t);
_Bool c32isxdigit(char32_t);
_Bool c32isalnum(char32_t);

char32_t c32sunescape(const char32_t *const, char32_t **);
