#include <stdio.h>
#include <stdlib.h>

#include "skull/common/str.h"

/*
Panic (exit with error msg), using `fmt` as the message, and `...` as the
`ErrorMsg` parameters.
*/
#define PANIC(fmt, ...) \
	fmt_error(fmt, (ErrorMsg[]){ \
		__VA_ARGS__, \
		{0} \
	}); \
	exit(1)

