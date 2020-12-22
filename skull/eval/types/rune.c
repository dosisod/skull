#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/eval/types/types.h"

#include "skull/eval/types/rune.h"

/*
Return rune type converted from `token`
*/
SkullRune eval_rune(const Token *const token) {
	const char32_t *start = token->begin + 1;
	const char32_t *copy = start;

	const char32_t *error = NULL;
	SkullRune ret = c32sunescape(&start, &error);

	if (error) {
		PANIC(ERR_BAD_ESCAPE, { .tok = token, .str = error });
	}

	if (start == copy) {
		ret = *start;
	}

	return ret;
}
