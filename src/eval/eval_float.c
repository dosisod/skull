#include <stddef.h>
#include <stdlib.h>

#include "../../src/errors.h"
#include "../../src/parse/classify.h"

#include "eval_float.h"

long double eval_float(const token_t *token, const char32_t **error) {
	if (token->token_type!=TOKEN_FLOAT_CONST) {
		*error=ERR_TYPE_MISMATCH;
		return 0.0L;
	}

	char *tmp=c32stombs(token->begin);
	const long double ret=strtold(tmp, NULL);

	free(tmp);
	return ret;
}
