#pragma once

#include <errno.h>
#include <limits.h>

#include "../../src/errors.h"
#include "../../src/parse/classify.h"
#include "../../src/parse/tokenize.h"

/*
Converts a `TOKEN_INT_CONST` token to an actual integer (`int64_t`).

`error` is `NULL` if no error occurs, else `error` points to error msg.
*/
int64_t eval_integer(const token_t *token, const char32_t **error) {
	if (token->token_type!=TOKEN_INT_CONST) {
		*error=ERROR_MSG[ERROR_TYPE_MISMATCH];
		return 0;
	}

	int64_t ret=0;
	const char32_t *begin=token->begin;
	uint8_t base=10;

	if (c32sncmp(U"0b", token->begin, 2)) {
		begin+=2;
		base=2;
	}
	else if (c32sncmp(U"0o", token->begin, 2)) {
		begin+=2;
		base=8;
	}
	else if (c32sncmp(U"0x", token->begin, 2)) {
		begin+=2;
		base=16;
	}

	errno=0;
	char *tmp=c32stombs(begin);
	ret=strtoll(tmp, NULL, base);
	free(tmp);

	if ((ret==LLONG_MAX || ret==LLONG_MIN) && errno==ERANGE) {
		*error=ERROR_MSG[ERROR_OVERFLOW];
	}

	return ret;
}
