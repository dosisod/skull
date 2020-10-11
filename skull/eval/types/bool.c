#include <stdbool.h>
#include <string.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/eval/types/types.h"
#include "skull/parse/classify.h"

#include "skull/eval/types/bool.h"

/*
Returns the string representation of boolean `var`
*/
char *fmt_bool_type(const Variable *const var) {
	bool data = false;
	variable_read(&data, var);

	char *const ret = strdup(data ? "true" : "false");
	DIE_IF_MALLOC_FAILS(ret);

	return ret;
}

/*
Return pointer to a bool, converted from `token`
*/
void *eval_bool(const Token *const token) {
	bool *ret;
	ret = malloc(sizeof *ret);
	DIE_IF_MALLOC_FAILS(ret);

	*ret = token_cmp(U"true", token);
	return ret;
}
