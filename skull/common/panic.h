#include <stdio.h>
#include <stdlib.h>

#include "skull/common/str.h"

#define PANIC(str) \
	(str); \
	exit(1)

#define PANIC_ON_ERR(str) \
	if (str) { \
		PANIC(str); \
	}
