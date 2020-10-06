#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/eval/types/types.h"
#include "skull/parse/classify.h"

#include "skull/eval/types/bool.h"

/*
Returns the string representation of boolean `var`
*/
char32_t *fmt_bool_type(const Variable *const var) {
	bool data = false;
	variable_read(&data, var);

	char32_t *const ret = c32sdup(data ? U"true" : U"false");
	DIE_IF_MALLOC_FAILS(ret);

	return ret;
}

/*
Return pointer to a bool, converted from `token`
*/
void *eval_bool(const Token *const token, const char32_t **error) {
	if (token->token_type != TOKEN_BOOL_CONST) {
		*error = FMT_ERROR(ERR_TYPE_MISMATCH, { .type = &TYPE_BOOL });
		return NULL;
	}
	bool *ret;
	ret = malloc(sizeof *ret);
	DIE_IF_MALLOC_FAILS(ret);

	*ret = token_cmp(U"true", token);
	return ret;
}
