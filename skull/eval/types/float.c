#include <errno.h>
#include <math.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/eval/types/sprint_fmt.h"
#include "skull/parse/classify.h"

#include "skull/eval/types/float.h"

/*
Returns a Skull float parsed from `token`.

`error` is `NULL` if no error occurs, else `error` points to error msg.
*/
SkullFloat eval_float(const Token *const token, char32_t **error) {
	char *const tmp = c32stombs(token->begin);

	if (strcmp("Infinity", tmp) == 0) {
		free(tmp);
		return HUGE_VAL;
	}
	if (strcmp("-Infinity", tmp) == 0) {
		free(tmp);
		return -HUGE_VAL;
	}

	errno = 0;
	SkullFloat ret = strtod(tmp, NULL);
	free(tmp);

	if (isinf(ret) && errno == ERANGE) {
		*error = FMT_ERROR(ERR_OVERFLOW, { .tok = token });
	}

	return ret;
}
