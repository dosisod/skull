#include <ctype.h>
#include <errno.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/compiler/types/types.h"

#include "skull/compiler/types/rune.h"

/*
Return rune type converted from `token`.
*/
SkullRune eval_rune(const Token *const token) {
	const char32_t *start = token->begin + 1;
	const char32_t *copy = start;

	if (iscntrl((int)*start)) {
		PANIC(ERR_NO_CONTROL_CHAR, { .loc = &token->location });
	}

	const char32_t *error = NULL;
	SkullRune ret = c32sunescape(&start, &error);

	mbstate_t mbs;
	memset(&mbs, 0, sizeof mbs);

	errno = 0;
	char *buf = alloca(MB_CUR_MAX);
	c32rtomb(buf, ret, &mbs);

	if (errno == EILSEQ) {
		PANIC(ERR_ILLEGAL_SEQ, { .loc = &token->location });
	}

	if (error) {
		PANIC(ERR_BAD_ESCAPE, { .loc = &token->location, .str = error });
	}

	if (start == copy) ret = *start;

	return ret;
}
