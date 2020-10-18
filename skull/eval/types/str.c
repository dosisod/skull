#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/eval/types/types.h"
#include "skull/parse/classify.h"

#include "skull/eval/types/str.h"

/*
Return Skull string converted from `token`
*/
SkullStr eval_str(const Token *const token, char32_t **error) {
	char32_t *str;
	str = malloc((token_len(token) - 1) * sizeof *str);
	DIE_IF_MALLOC_FAILS(str);

	const char32_t *tmp = token->begin + 1;
	size_t wrote = 0;
	while (*tmp && tmp < token->end - 1) {
		const char32_t try_escape = c32sunescape(tmp, error);
		if (*error) {
			free(str);
			return NULL;
		}
		if (try_escape) {
			str[wrote] = try_escape;
			tmp++;
		}
		else {
			str[wrote] = *tmp;
		}

		tmp++;
		wrote++;
	}
	str[wrote] = '\0';

	return str;
}
