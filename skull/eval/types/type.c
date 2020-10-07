#define _XOPEN_SOURCE 700
#include <string.h>
#undef _XOPEN_SOURCE

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"

#include "skull/eval/types/type.h"

/*
Returns the string representation of type variable `var`
*/
char *fmt_type_type(const Variable *const var) {
	const Type *type = NULL;
	variable_read(&type, var);

	return strdup(type->name);
}

/*
Return pointer to a variable type, converted from `token`
*/
void *eval_type(const Token *const token, const char32_t **error) {
	Type **type = malloc(sizeof(Type *));
	DIE_IF_MALLOC_FAILS(type);

	char32_t *const tmp_name = token_str(token);
	char *const type_name = c32stombs(tmp_name);
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
