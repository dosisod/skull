#pragma once

#include <stdio.h>

#define DIE_IF_MALLOC_FAILS(ptr) \
	if ((ptr)==NULL) { \
		puts("\nfailed to allocate sufficient memory, exiting"); \
		exit(1); \
	}
