#include <errno.h>
#include <limits.h>

#include "skull/common/errors.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"

#include "skull/eval/types/int.h"

/*
Returns an Skull integer parsed from `token`.
*/
SkullInt eval_integer(const Token *const token) {
	const char32_t *begin = token->begin;
	int base = 10;

	if (*begin == '0') {
		const char32_t modifier = begin[1];
		begin += 2;

		if (modifier == 'b') {
			base = 2;
		}
		else if (modifier == 'o') {
			base = 8;
		}
		else if (modifier == 'x') {
			base = 16;
		}
		else {
			begin -= 2;
		}
	}

	char *const num_str = c32stombs(begin);

	errno = 0;
	SkullInt ret = strtoll(num_str, NULL, base);
	free(num_str);

	if ((ret == LLONG_MAX || ret == LLONG_MIN) && errno == ERANGE) {
		PANIC(ERR_OVERFLOW, { .tok = token });
	}

	return ret;
}
