#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include "skull/common/errors.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/parse/classify.h"

#include "skull/eval/types/int.h"

/*
Returns an Skull integer parsed from `token`.
*/
SkullInt eval_integer(const Token *const token) {
	const char32_t *begin = token->begin;
	int base = 10;

	if (c32sncmp(U"0b", token->begin, 2)) {
		begin += 2;
		base = 2;
	}
	else if (c32sncmp(U"0o", token->begin, 2)) {
		begin += 2;
		base = 8;
	}
	else if (c32sncmp(U"0x", token->begin, 2)) {
		begin += 2;
		base = 16;
	}

	char *const num_str = c32stombs(begin);

	errno = 0;
	SkullInt ret = strtoll(num_str, NULL, base);
	free(num_str);

	if ((ret == LLONG_MAX || ret == LLONG_MIN) && errno == ERANGE) {
		PANIC(FMT_ERROR(ERR_OVERFLOW, { .tok = token }));
	}

	return ret;
}
