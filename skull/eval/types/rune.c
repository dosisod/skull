#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/eval/types/types.h"
#include "skull/parse/classify.h"

#include "skull/eval/types/rune.h"

/*
Return rune type converted from `token`
*/
SkullRune eval_rune(const Token *const token) {
	char32_t *error = NULL;
	SkullRune ret = c32sunescape(token->begin + 1, &error);
	PANIC_ON_ERR(error);

	if (!ret) {
		ret = token->begin[1];
	}

	return ret;
}
