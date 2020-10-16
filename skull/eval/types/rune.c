#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/eval/types/types.h"
#include "skull/parse/classify.h"

#include "skull/eval/types/rune.h"

/*
Returns the string representation of rune `var`
*/
char *fmt_rune_type(const Variable *const var) {
	char32_t *tmp;
	tmp = malloc(2 * sizeof *tmp);
	DIE_IF_MALLOC_FAILS(tmp);

	variable_read(tmp, var);
	tmp[1] = '\0';

	char *ret = c32stombs(tmp);
	free(tmp);
	return ret;
}

/*
Return rune type converted from `token`
*/
SkullRune eval_rune(const Token *const token, char32_t **error) {
	SkullRune ret = c32sunescape(token->begin + 1, error);
	if (*error) {
		return '\0';
	}
	if (!ret) {
		ret = token->begin[1];
	}

	return ret;
}
