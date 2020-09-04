#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/errors.h"

#include "skull/eval/types/type.h"

/*
Returns the string representation of type variable `var`
*/
char32_t *fmt_type_type(const Variable *var) {
	Type *type = NULL;
	variable_read(&type, var);

	char32_t *ret = mbstoc32s(type->name);
	DIE_IF_MALLOC_FAILS(ret);

	return ret;
}

/*
Return pointer to a variable type, converted from `token`
*/
void *eval_type(const Token *token, const char32_t **error) {
	Type **type = malloc(sizeof(Type *));
	DIE_IF_MALLOC_FAILS(type);

	char32_t *tmp_name = token_str(token);
	char *type_name = c32stombs(tmp_name);
	free(tmp_name);

	*type = find_type(type_name);
	free(type_name);

	if (*type == &TYPE_TYPE) {
		*error = ERR_TYPE_TYPE_BAD;
		free(type);
		return NULL;
	}

	return type;
}
