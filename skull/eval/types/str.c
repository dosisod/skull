#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/parse/classify.h"

#include "skull/eval/types/str.h"

/*
Returns the string representation of string `var`
*/
char32_t *fmt_str_type(const variable_t *var) {
	const char32_t *str=NULL;
	variable_read(&str, var);

	char32_t *ret=c32sdup(str);
	DIE_IF_MALLOC_FAILS(ret);

	return ret;
}

/*
Concatenate `lhs` and `rhs` strings
*/
variable_t *add_str_type(const variable_t *lhs, const variable_t *rhs) {
	variable_t *ret=make_variable(U"str", U"tmp", false);
	if (!ret) {
		return NULL;
	}

	char32_t *lhs_tmp=NULL;
	variable_read(&lhs_tmp, lhs);
	char32_t *rhs_tmp=NULL;
	variable_read(&rhs_tmp, rhs);

	char32_t *cat=c32scat(lhs_tmp, rhs_tmp);
	DIE_IF_MALLOC_FAILS(cat);

	variable_write(ret, &cat);
	return ret;
}

/*
Return pointer to string, converted from `token`
*/
void *eval_str(const token_t *token, const char32_t **error) {
	if (token->token_type!=TOKEN_STR_CONST) {
		*error=ERR_TYPE_MISMATCH;
		return NULL;
	}

	char32_t *tmp=token_str(token);

	char32_t **ret=malloc(sizeof(char32_t *));
	DIE_IF_MALLOC_FAILS(ret);
	*ret=tmp;

	return ret;
}
