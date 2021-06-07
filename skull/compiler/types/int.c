#include <errno.h>
#include <limits.h>
#include <stdbool.h>

#include "skull/common/errors.h"
#include "skull/compiler/types/_underscore_num.h"

#include "skull/compiler/types/int.h"

/*
Returns a 64 bit integer parsed from `token`.
*/
int64_t eval_integer(const Token *const token, bool *err) {
	const char32_t *begin = token->begin;
	int base = 10;

	if (*begin == '0') {
		const char32_t modifier = begin[1];
		begin += 2;

		if (modifier == 'b') base = 2;
		else if (modifier == 'o') base = 8;
		else if (modifier == 'x') base = 16;
		else begin -= 2;
	}

	// create a dummy token since we cannot modify `token`
	// but still need to advance the `begin` field
	char *num_str = token_mbs_str(&(Token){
		.begin = begin, .end = token->end
	});

	strip_underscore_num(num_str, 0);

	errno = 0;
	int64_t ret = strtoll(num_str, NULL, base);
	free(num_str);

	if ((ret == LLONG_MAX || ret == LLONG_MIN) && errno == ERANGE) {
		FMT_ERROR(ERR_OVERFLOW, { .tok = token });

		*err = true;
		return 0;
	}

	return ret;
}
