#pragma once

#include <stdio.h>
#include <stdlib.h>

#define DIE_IF_MALLOC_FAILS(ptr) \
	if (!(ptr)) { \
		puts("\nfailed to allocate sufficient memory, exiting"); \
		exit(1); \
	}
