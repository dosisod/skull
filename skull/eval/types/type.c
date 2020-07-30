#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/errors.h"

#include "skull/eval/types/type.h"

char32_t *fmt_type_type(const variable_t *var) {
	type_t *type=NULL;
	variable_read(&type, var);

	char32_t *ret=c32sdup(type->name);
	DIE_IF_MALLOC_FAILS(ret);

	return ret;
}

void *eval_type(const token_t *token, const char32_t **error) {
	type_t **type=malloc(sizeof(type_t *));
	DIE_IF_MALLOC_FAILS(type);

	MAKE_TOKEN_BUF(buf, token);
	*type=find_type(buf);

	if (*type==&TYPE_TYPE) {
		*error=ERR_TYPE_TYPE_BAD;
		free(type);
		return NULL;
	}

	return type;
}
