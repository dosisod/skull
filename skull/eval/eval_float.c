#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/malloc.h"
#include "skull/errors.h"
#include "skull/parse/classify.h"

#include "skull/eval/eval_float.h"

/*
Converts a `TOKEN_FLOAT_CONST` token to a floating point number pointer (`double *`).

`error` is `NULL` if no error occurs, else `error` points to error msg.
*/
void *eval_float(const token_t *token, const char32_t **error) {
	if (token->token_type != TOKEN_FLOAT_CONST) {
		*error = ERR_TYPE_MISMATCH;
		return NULL;
	}

	char *tmp = c32stombs(token->begin);

	double *ret;
	ret = malloc(sizeof *ret);
	DIE_IF_MALLOC_FAILS(ret);

	if (strcmp("Infinity", tmp) == 0) {
		*ret = HUGE_VAL;
		return ret;
	}
	if (strcmp("-Infinity", tmp) == 0) {
		*ret = -HUGE_VAL;
		return ret;
	}

	errno = 0;
	*ret = strtod(tmp, NULL);
	free(tmp);

	if (isinf(*ret) && errno == ERANGE) {
		*error = FMT_ERROR(ERR_OVERFLOW, { .tok = token });
	}

	return ret;
}
