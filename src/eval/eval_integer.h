#pragma once

#include <errno.h>
#include <limits.h>

#include "../../src/errors.h"
#include "../../src/parse/classify.h"
#include "../../src/parse/tokenize.h"

/*
Converts a `TOKEN_INT_CONST` token to an actual integer (`int64_t`).

Return `NULL` if no errors occurred, else, pointer to error msg.
*/
int64_t eval_integer(const token_t *token, const wchar_t **error) {
	if (token->token_type!=TOKEN_INT_CONST) {
		*error=ERROR_MSG[ERROR_TYPE_MISMATCH];
		return 0;
	}

	int64_t ret=0;
	const wchar_t *begin=token->begin;
	uint8_t base=10;

	if (wcsncmp(L"0b", token->begin, 2)==0) {
		begin+=2;
		base=2;
	}
	else if (wcsncmp(L"0o", token->begin, 2)==0) {
		begin+=2;
		base=8;
	}
	else if (wcsncmp(L"0x", token->begin, 2)==0) {
		begin+=2;
		base=16;
	}

	errno=0;
	ret=wcstoll(begin, NULL, base);

	if ((ret==LLONG_MAX || ret==LLONG_MIN) && errno==ERANGE) {
		*error=ERROR_MSG[ERROR_OVERFLOW];
	}

	return ret;
}
