#pragma once

#include <stdbool.h>
#include <string.h>

/*
Copies string with `strncpy`, then set last char to NULL
*/
char *strncpyz(char *dest, const char *src, size_t n) {
	char *ret=strncpy(dest, src, n);
	dest[n]='\0';

	return ret;
}

/*
Same as calling strcmp(s1, s2)==0
*/
bool samestr(const char *s1, const char *s2) {
	return strcmp(s1, s2)==0;
}