#include <stdio.h>
#include <stdlib.h>

#include "skull/common/str.h"

#define PANIC(str) \
	char *const panic_str = c32stombs(str); \
	printf("Compilation error: %s\n", panic_str); \
	free(panic_str); \
	exit(1)

#define PANIC_ON_ERR(str) \
	if (str) { \
		PANIC(str); \
	}
