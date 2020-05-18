#pragma once

#include <errno.h>
#include <limits.h>

#include "../../src/parse/classify.h"
#include "../../src/parse/tokenize.h"

#define EVAL_INTEGER_OK 0
#define EVAL_INTEGER_ERR 1

/*
Converts a `TOKEN_INT_CONST` token to an actual integer (long long int).

If an error occurs while converting, `error` is set to `EVAL_INTEGER_ERR`.
`error` should always be `EVAL_INTEGER_OK` upon success.
*/
long long int eval_integer(const token_t *token, short int *error) {
	*error=EVAL_INTEGER_OK;

	if (token->token_type!=TOKEN_INT_CONST) {
		*error=EVAL_INTEGER_ERR;
		return 0;
	}

	long long int ret=0;
	const wchar_t *begin=token->begin;
	short int base=10;

	if (wcsncmp(L"0b", token->begin, 2)==0) {
		begin+=2;
		base=2;
	}
	else if (wcsncmp(L"0x", token->begin, 2)==0) {
		begin+=2;
		base=16;
	}

	errno=0;
	ret=wcstoll(begin, NULL, base);

	if ((ret==LLONG_MAX || ret==LLONG_MIN) && errno==ERANGE) {
		*error=EVAL_INTEGER_ERR;
	}

	return ret;
}
