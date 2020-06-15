#pragma once

#define DIE_IF_MALLOC_FAILS(ptr) \
	if ((ptr)==NULL) { \
		wprintf(L"\nfailed to allocate sufficient memory, exiting\n"); \
		exit(1); \
	}
