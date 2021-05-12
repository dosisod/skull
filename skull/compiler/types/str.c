#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/compiler/types/types.h"

#include "skull/compiler/types/str.h"

/*
Return string converted from `token`
*/
char32_t *eval_str(const Token *const token, bool *err) {
	char32_t *copy;
	copy = Malloc((token_len(token) - 1) * sizeof *copy);

	const char32_t *str = token->begin + 1;
	size_t wrote = 0;
	while (*str && str < token->end - 1) {
		const char32_t *original = str;
		const char32_t *error = NULL;
		copy[wrote] = c32sunescape(&str, &error);

		if (error) {
			FMT_ERROR(ERR_BAD_ESCAPE, { .loc = &token->location, .str = error });

			free(copy);
			*err = true;
			return NULL;
		}

		if (str == original) copy[wrote] = *str;

		str++;
		wrote++;
	}
	copy[wrote] = '\0';

	return Realloc(copy, (wrote + 1) * sizeof(char32_t));
}
