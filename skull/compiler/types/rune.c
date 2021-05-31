#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/compiler/types/types.h"

#include "skull/compiler/types/rune.h"

/*
Return "rune" converted from `token`.
*/
char32_t eval_rune(const Token *const token, bool *err) {
	const char32_t *start = token->begin + 1;
	const char32_t *copy = start;

	if (iscntrl((int)*start)) {
		FMT_ERROR(ERR_NO_CONTROL_CHAR, { .loc = &token->location });

		*err = true;
		return '\0';
	}

	const char32_t *error = NULL;
	char32_t ret = c32sunescape(&start, &error);

	mbstate_t mbs;
	memset(&mbs, 0, sizeof mbs);

	errno = 0;
	char *buf = alloca(MB_CUR_MAX);
	c32rtomb(buf, ret, &mbs);

	if (errno == EILSEQ) {
		FMT_ERROR(ERR_ILLEGAL_SEQ, { .loc = &token->location });

		*err = true;
		return '\0';
	}

	if (error) {
		FMT_ERROR(ERR_BAD_ESCAPE, { .loc = &token->location, .str32 = error });

		*err = true;
		return '\0';
	}

	if (start == copy) ret = *start;

	return ret;
}
