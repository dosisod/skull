#include "skull/common/str.h"
#include "skull/common/malloc.h"
#include "skull/eval/variable.h"

char32_t *fmt_type_type(const variable_t *var) {
	type_t *type=NULL;
	variable_read(&type, var);

	char32_t *ret=c32sdup(type->name);
	DIE_IF_MALLOC_FAILS(ret);

	return ret;
}
