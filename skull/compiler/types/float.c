#include <errno.h>
#include <math.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/compiler/types/_underscore_num.h"

#include "skull/compiler/types/float.h"

/*
Returns a Skull float parsed from `token`.
*/
SkullFloat eval_float(const Token *const token, bool *err) {
	char *const float_str = token_mbs_str(token);

	if (strcmp("Infinity", float_str) == 0) {
		free(float_str);
		return (double)INFINITY;
	}
	if (strcmp("-Infinity", float_str) == 0) {
		free(float_str);
		return (double)-INFINITY;
	}
	if (strcmp("NaN", float_str) == 0) {
		free(float_str);
		return (double)NAN;
	}

	strip_underscore_num(float_str, '.');

	errno = 0;
	SkullFloat ret = strtod(float_str, NULL);
	free(float_str);

	if (isinf(ret) && errno == ERANGE) {
		FMT_ERROR(ERR_OVERFLOW, { .tok = token });

		*err = true;
		return 0;
	}

	return ret;
}
