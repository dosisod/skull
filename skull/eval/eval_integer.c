#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/eval/types/defs.h"
#include "skull/eval/types/types.h"
#include "skull/parse/classify.h"

#include "skull/eval/eval_integer.h"

/*
Converts a `TOKEN_INT_CONST` token to an integer pointer (`SkullInt *`).

`error` is `NULL` if no error occurs, else `error` points to error msg.
*/
void *eval_integer(const Token *token, const char32_t **error) {
	if (token->token_type != TOKEN_INT_CONST) {
		*error = FMT_ERROR(ERR_TYPE_MISMATCH, { .type = &TYPE_INT });
		return NULL;
	}

	SkullInt *ret;
	ret = malloc(sizeof *ret);
	DIE_IF_MALLOC_FAILS(ret);

	const char32_t *begin = token->begin;
	int base = 10; // NOLINT

	if (c32sncmp(U"0b", token->begin, 2)) {
		begin += 2;
		base = 2;
	}
	else if (c32sncmp(U"0o", token->begin, 2)) {
		begin += 2;
		base = 8; // NOLINT
	}
	else if (c32sncmp(U"0x", token->begin, 2)) {
		begin += 2;
		base = 16; // NOLINT
	}

	char *tmp = c32stombs(begin);

	errno = 0;
	*ret = strtoll(tmp, NULL, base);
	free(tmp);

	if ((*ret == LLONG_MAX || *ret == LLONG_MIN) && errno == ERANGE) {
		*error = FMT_ERROR(ERR_OVERFLOW, { .tok = token });
	}

	return ret;
}
