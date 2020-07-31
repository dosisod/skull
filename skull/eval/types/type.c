#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/errors.h"

#include "skull/eval/types/type.h"

/*
Returns the string representation of type variable `var`
*/
char32_t *fmt_type_type(const variable_t *var) {
	type_t *type=NULL;
	variable_read(&type, var);

	char32_t *ret=c32sdup(type->name);
	DIE_IF_MALLOC_FAILS(ret);

	return ret;
}

/*
Return pointer to a variable type, converted from `token`
*/
void *eval_type(const token_t *token, const char32_t **error) {
	type_t **type=malloc(sizeof(type_t *));
	DIE_IF_MALLOC_FAILS(type);

	char32_t *type_name=token_str(token);
	*type=find_type(type_name);
	free(type_name);

	if (*type==&TYPE_TYPE) {
		*error=ERR_TYPE_TYPE_BAD;
		free(type);
		return NULL;
	}

	return type;
}
