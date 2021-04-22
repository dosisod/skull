#include <stdio.h>
#include <stdlib.h>

#include "skull/common/str.h"

/*
Panic (exit with error msg), using `fmt` as the message, and `...` as the
`ErrorMsg` parameters.
*/
#define PANIC(fmt, ...) \
	FMT_ERROR(fmt, __VA_ARGS__); \
	exit(1)

