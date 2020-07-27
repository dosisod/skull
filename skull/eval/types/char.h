#include "skull/common/str.h"
#include "skull/common/malloc.h"
#include "skull/eval/variable.h"

char32_t *fmt_char_type(const variable_t *var) {
	char32_t *ret=malloc(sizeof(char32_t) * 2);
	DIE_IF_MALLOC_FAILS(ret);

	variable_read(ret, var);
	ret[1]=U'\0';

	return ret;
}
