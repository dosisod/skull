#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/malloc.h"
#include "skull/errors.h"
#include "skull/parse/classify.h"

#include "skull/eval/eval_float.h"

/*
Converts a `TOKEN_FLOAT_CONST` token to a floating point number pointer (`long double *`).

`error` is `NULL` if no error occurs, else `error` points to error msg.
*/
void *eval_float(const token_t *token, const char32_t **error) {
	if (token->token_type!=TOKEN_FLOAT_CONST) {
		*error=ERR_TYPE_MISMATCH;
		return NULL;
	}

	char *tmp=c32stombs(token->begin);

	long double *ret=malloc(sizeof(long double));
	DIE_IF_MALLOC_FAILS(ret);

	if (strcmp("Infinity", tmp)==0) {
		*ret=HUGE_VALL;
		return ret;
	}
	if (strcmp("-Infinity", tmp)==0) {
		*ret=-HUGE_VALL;
		return ret;
	}

	*ret=strtold(tmp, NULL);

	free(tmp);
	return ret;
}
