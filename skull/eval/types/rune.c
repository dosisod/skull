#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/eval/types/defs.h"
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

	return c32stombs(tmp);
}

/*
Return pointer to a rune type, converted from `token`
*/
void *eval_rune(const Token *const token, const char32_t **error) {
	if (token->token_type != TOKEN_RUNE_CONST) {
		*error = FMT_ERROR(ERR_TYPE_MISMATCH, { .type = &TYPE_FLOAT });
		return NULL;
	}

	SkullRune *ret;
	ret = malloc(sizeof *ret);
	DIE_IF_MALLOC_FAILS(ret);

	*ret = c32sunescape(token->begin + 1, error);
	if (*error) {
		free(ret);
		return NULL;
	}
	if (!*ret) {
		*ret = token->begin[1];
	}

	return ret;
}
