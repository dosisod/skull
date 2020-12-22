#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"

#include "skull/eval/types/float.h"

/*
Returns a Skull float parsed from `token`.
*/
SkullFloat eval_float(const Token *const token) {
	char *const float_str = c32stombs(token->begin);

	if (strcmp("Infinity", float_str) == 0) {
		free(float_str);
		return HUGE_VAL;
	}
	if (strcmp("-Infinity", float_str) == 0) {
		free(float_str);
		return -HUGE_VAL;
	}

	errno = 0;
	SkullFloat ret = strtod(float_str, NULL);
	free(float_str);

	if (isinf(ret) && errno == ERANGE) {
		PANIC(ERR_OVERFLOW, { .tok = token });
	}

	return ret;
}
