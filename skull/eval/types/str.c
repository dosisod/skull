#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/eval/types/types.h"
#include "skull/parse/classify.h"

#include "skull/eval/types/str.h"

/*
Return Skull string converted from `token`
*/
SkullStr eval_str(const Token *const token) {
	char32_t *copy;
	copy = malloc((token_len(token) - 1) * sizeof *copy);
	DIE_IF_MALLOC_FAILS(copy);

	const char32_t *str = token->begin + 1;
	size_t wrote = 0;
	while (*str && str < token->end - 1) {
		char32_t *error = NULL;
		const char32_t try_escape = c32sunescape(&str, &error);

		PANIC_ON_ERR(error);

		if (try_escape) {
			copy[wrote] = try_escape;
		}
		else {
			copy[wrote] = *str;
		}

		str++;
		wrote++;
	}
	copy[wrote] = '\0';

	return copy;
}
