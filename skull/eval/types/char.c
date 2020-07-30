#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/parse/classify.h"

#include "skull/eval/types/char.h"

char32_t *fmt_char_type(const variable_t *var) {
	char32_t *ret=malloc(sizeof(char32_t) * 2);
	DIE_IF_MALLOC_FAILS(ret);

	variable_read(ret, var);
	ret[1]=U'\0';

	return ret;
}

void *eval_char(const token_t *token, const char32_t **error) {
	if (token->token_type!=TOKEN_CHAR_CONST) {
		*error=ERR_TYPE_MISMATCH;
		return NULL;
	}

	char32_t *ret=malloc(sizeof(char32_t));
	DIE_IF_MALLOC_FAILS(ret);

	*ret=*token->begin;
	return ret;
}
