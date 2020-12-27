#include <stdio.h>

#include "skull/common/malloc.h"

/*
Exit if malloc fails to allocate memory.
*/
#define DIE_IF_MALLOC_FAILS(ptr) \
	if (!(ptr)) { \
		puts("\nfailed to allocate sufficient memory, exiting"); \
		exit(1); \
	}

/*
`NULL` safe wrapper for `malloc`
*/
void *Malloc(size_t size) {
	void *ptr = malloc(size);
	DIE_IF_MALLOC_FAILS(ptr);

	return ptr;
}

/*
`NULL` safe wrapper for `calloc`
*/
void *Calloc(size_t nmemb, size_t size) {
	void *ptr = calloc(nmemb, size);
	DIE_IF_MALLOC_FAILS(ptr);

	return ptr;
}

/*
`NULL` safe wrapper for `realloc`
*/
void *Realloc(void *ptr, size_t size) {
	void *new_ptr = realloc(ptr, size);
	DIE_IF_MALLOC_FAILS(new_ptr);

	return new_ptr;
}
