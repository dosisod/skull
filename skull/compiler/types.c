#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "skull/codegen/shared.h"
#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"

#include "skull/compiler/types.h"


/*
Return bool converted from `token`
*/
bool eval_bool(const Token *const token) {
	// since token is either "true" or "false", just check length of token
	return token_len(token) == 4;
}

/*
Returns a float (actually a double) parsed from `token`.
*/
double eval_float(const Token *const token, bool *err) {
	char *const float_str = token_mbs_str(token);

	if (strcmp("Infinity", float_str) == 0) {
		free(float_str);
		return (double)INFINITY;
	}
	if (strcmp("-Infinity", float_str) == 0) {
		free(float_str);
		return (double)-INFINITY;
	}
	if (strcmp("NaN", float_str) == 0) {
		free(float_str);
		return (double)NAN;
	}

	strip_underscore_num(float_str, '.');

	errno = 0;
	double ret = strtod(float_str, NULL);
	free(float_str);

	if (isinf(ret) && errno == ERANGE) {
		FMT_ERROR(ERR_OVERFLOW, { .tok = token });

		*err = true;
		return 0;
	}

	return ret;
}

/*
Returns a 64 bit integer parsed from `token`.
*/
int64_t eval_integer(const Token *const token, bool *err) {
	const char32_t *begin = token->begin;
	int base = 10;

	if (*begin == '0') {
		const char32_t modifier = begin[1];
		begin += 2;

		if (modifier == 'b') base = 2;
		else if (modifier == 'o') base = 8;
		else if (modifier == 'x') base = 16;
		else begin -= 2;
	}

	// create a dummy token since we cannot modify `token`
	// but still need to advance the `begin` field
	char *num_str = token_mbs_str(&(Token){
		.begin = begin, .end = token->end
	});

	strip_underscore_num(num_str, 0);

	errno = 0;
	int64_t ret = strtoll(num_str, NULL, base);
	free(num_str);

	if ((ret == LLONG_MAX || ret == LLONG_MIN) && errno == ERANGE) {
		FMT_ERROR(ERR_OVERFLOW, { .tok = token });

		*err = true;
		return 0;
	}

	return ret;
}

/*
Return "rune" converted from `token`.
*/
char32_t eval_rune(const Token *const token, bool *err) {
	const char32_t *start = token->begin + 1;

	if (iscntrl((int)*start)) {
		FMT_ERROR(ERR_NO_CONTROL_CHAR, { .loc = &token->location });

		*err = true;
		return '\0';
	}

	const char32_t *error = NULL;
	char32_t ret = c32sunescape(&start, &error);

	mbstate_t mbs;
	memset(&mbs, 0, sizeof mbs);

	errno = 0;
	char *buf = alloca(MB_CUR_MAX);
	c32rtomb(buf, ret, &mbs);

	if (errno == EILSEQ) {
		FMT_ERROR(ERR_ILLEGAL_SEQ, { .loc = &token->location });

		*err = true;
		return '\0';
	}

	if (error) {
		FMT_ERROR(ERR_BAD_ESCAPE, { .loc = &token->location, .str32 = error });

		*err = true;
		return '\0';
	}

	return ret;
}

/*
Return string converted from `token`, or `NULL` if an error occurred.
*/
char32_t *eval_str(const Token *const token) {
	char32_t *copy;
	copy = Malloc((token_len(token) - 1) * sizeof *copy);

	const char32_t *str = token->begin + 1;
	size_t wrote = 0;
	while (*str && str < token->end - 1) {
		const char32_t *error = NULL;
		copy[wrote] = c32sunescape(&str, &error);

		if (error) {
			FMT_ERROR(ERR_BAD_ESCAPE, {
				.loc = &token->location, .str32 = error
			});

			free(copy);
			return NULL;
		}

		str++;
		wrote++;
	}
	copy[wrote] = '\0';

	return Realloc(copy, (wrote + 1) * sizeof(char32_t));
}

/*
Returns pointer to type with name `name`.
*/
Type __attribute__((pure)) find_type(const char *const name) {
	Type alias = ht_get(SKULL_STATE.type_aliases, name);
	if (alias) return alias;

	Type **type = TYPES_BUILTIN;

	while (*type) {
		if (strcmp(name, **type) == 0) return **type;

		type++;
	}

	return NULL;
}

Type TYPE_BOOL = "Bool";
Type TYPE_INT = "Int";
Type TYPE_FLOAT = "Float";
Type TYPE_RUNE = "Rune";
Type TYPE_STR = "Str";
Type TYPE_VOID = "Void";

Type **TYPES_BUILTIN = (Type *[]){
	&TYPE_BOOL,
	&TYPE_INT,
	&TYPE_FLOAT,
	&TYPE_RUNE,
	&TYPE_STR,
	&TYPE_VOID,
	NULL
};

void free_ht_type_alias(HashItem *item) {
	free((char *)item->key);
}

/*
Strip underscores from `str`.

This function modifies the contents at `str`.

If `c` is defined, stop iterating when `c` is found.
*/
void strip_underscore_num(char *str, char c) {
	char *probe = str;

	while (*probe && *probe != c) {
		if (*probe == '_') probe++;

		*str = *probe;
		str++;
		probe++;
	}

	// copy rest of string if `c` was hit
	while (*probe) {
		*str = *probe;
		str++;
		probe++;
	}
	*str = '\0';
}
