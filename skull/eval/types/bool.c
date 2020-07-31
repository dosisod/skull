#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/parse/classify.h"

#include "skull/eval/types/bool.h"

/*
Returns the string representation of boolean `var`
*/
char32_t *fmt_bool_type(const variable_t *var) {
	bool data=false;
	variable_read(&data, var);

	char32_t *ret=c32sdup(data ? U"true" : U"false");
	DIE_IF_MALLOC_FAILS(ret);

	return ret;
}

/*
Return pointer to a bool, converted from `token`
*/
void *eval_bool(const token_t *token, const char32_t **error) {
	if (token->token_type!=TOKEN_BOOL_CONST) {
		*error=ERR_TYPE_MISMATCH;
		return NULL;
	}
	bool *ret=malloc(sizeof(bool));
	DIE_IF_MALLOC_FAILS(ret);

	*ret=token_cmp(U"true", token);
	return ret;
}
