#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/parse/classify.h"

#include "skull/eval/types/str.h"

char32_t *fmt_str_type(const variable_t *var) {
	const char32_t *str=NULL;
	variable_read(&str, var);

	char32_t *ret=c32sdup(str);
	DIE_IF_MALLOC_FAILS(ret);

	return ret;
}

variable_t *add_str_type(const variable_t *lhs, const variable_t *rhs) {
	variable_t *ret=make_variable(U"str", U"tmp", false);
	if (ret==NULL) {
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

void *eval_str(const token_t *token, const char32_t **error) {
	if (token->token_type!=TOKEN_STR_CONST) {
		*error=ERR_TYPE_MISMATCH;
		return NULL;
	}

	MAKE_TOKEN_BUF(buf, token);
	char32_t *tmp=c32sdup(buf);

	char32_t **ret=malloc(sizeof(char32_t *));
	DIE_IF_MALLOC_FAILS(ret);
	*ret=tmp;

	return ret;
}