#include <stdio.h>
#include <stdlib.h>

#include "skull/common/str.h"

#define PANIC(fmt, ...) \
	fmt_error(fmt, (ErrorMsg[]){ \
		__VA_ARGS__, \
		{0} \
	}); \
	exit(1)


#define PANIC_ON_ERR(fmt, ...) \
	if (str) { \
		PANIC(str, __VA_ARGS__); \
	}
