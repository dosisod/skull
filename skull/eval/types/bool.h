#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/eval/variable.h"

char32_t *fmt_bool_type(const variable_t *var) {
	bool data=false;
	variable_read(&data, var);

	char32_t *ret=c32sdup(data ? U"true" : U"false");
	DIE_IF_MALLOC_FAILS(ret);

	return ret;
}
