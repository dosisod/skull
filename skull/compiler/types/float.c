#include <errno.h>
#include <math.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/compiler/types/_underscore_num.h"

#include "skull/compiler/types/float.h"

/*
Returns a Skull float parsed from `token`.
*/
SkullFloat eval_float(const Token *const token) {
	char *const float_str = token_mbs_str(token);

	if (strcmp("Infinity", float_str) == 0) {
		free(float_str);
		return HUGE_VAL;
	}
	if (strcmp("-Infinity", float_str) == 0) {
		free(float_str);
		return -HUGE_VAL;
	}

	strip_underscore_num(float_str, '.');

	errno = 0;
	SkullFloat ret = strtod(float_str, NULL);
	free(float_str);

	if (isinf(ret) && errno == ERANGE) {
		PANIC(ERR_OVERFLOW, { .tok = token });
	}

	return ret;
}