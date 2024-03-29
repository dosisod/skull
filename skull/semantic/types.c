#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/parse/ast_node.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/symbol.h"

#include "skull/semantic/types.h"

static void strip_underscore_num(char *, char);
static Type **TYPES_BUILTIN;


/*
Return bool converted from `token`
*/
bool eval_bool(const Token *const token) {
	// since token is either "true" or "false", just check length of token
	return token->len == 4;
}

/*
Returns a float (actually a double) parsed from `token`.
*/
double eval_float(const Token *const token, bool *err) {
	if (token_cmp(U"Infinity", token)) return (double)INFINITY;
	if (token_cmp(U"-Infinity", token)) return (double)-INFINITY;
	if (token_cmp(U"NaN", token)) return (double)NAN;

	char *float_str = token_to_mbs_str(token);
	strip_underscore_num(float_str, '.');

	errno = 0;
	const double ret = strtod(float_str, NULL);
	free(float_str);

	if (errno == ERANGE) {
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
	char *num_str = token_to_mbs_str(&(Token){
		.begin = begin, .len = token->len
	});

	strip_underscore_num(num_str, 0);

	errno = 0;
	const int64_t ret = strtoll(num_str, NULL, base);
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
	const char32_t ret = c32sunescape(&start, &error);

	if (error) {
		FMT_ERROR(ERR_BAD_ESCAPE, { .loc = &token->location, .str32 = error });

		*err = true;
		return '\0';
	}

	if (start[1] != '\'') {
		*err = true;
		FMT_ERROR(ERR_RUNE_TOO_LONG, { .loc = &token->location });
		return '\0';
	}

	mbstate_t mbs;
	memset(&mbs, 0, sizeof mbs);


	// Since MB_CUR_MAX cannot be used (would result in VLA), 16 is used
	// instead. MB_CUR_MAX on my system is 6, so I think 16 should be safe
	char buf[16];
	errno = 0;
	c32rtomb(buf, ret, &mbs);

	if (errno == EILSEQ) {
		FMT_ERROR(ERR_ILLEGAL_SEQ, { .loc = &token->location });

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
	copy = Malloc((token->len - 1) * sizeof *copy);

	const char32_t *str = token->begin + 1;
	size_t wrote = 0;
	while (*str && str < token->begin + token->len - 1) {
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
const Type __attribute__((pure)) *find_type(
	SemanticState *state,
	char *const name
) {
	Symbol *symbol = scope_find_name(state->scope, name);

	if (symbol && symbol->type == SYMBOL_ALIAS) return symbol->expr_type;

	return find_builtin_type(name);
}

const Type __attribute__((pure)) *find_builtin_type(const char *const name) {
	Type **type = TYPES_BUILTIN;

	while (*type) {
		if (strcmp(name, (*type)->name) == 0) return *type;

		type++;
	}

	return NULL;
}

Type *get_reference_type(SemanticState *state, const Type *type) {
	if (!type) return NULL;

	if (!state->dynamic_refs) state->dynamic_refs = make_ht();

	Type *found = ht_get(state->dynamic_refs, type->name);
	if (found) return found;

	const size_t len = strlen(type->name);
	char *name = Malloc(len + 2);
	name[0] = '&';
	strncpy(name + 1, type->name, len + 1);

	Type *new_type = Calloc(1, sizeof(Type));
	*new_type = (Type){ .name = name, .inner = type };

	ht_add(state->dynamic_refs, type->name, new_type);

	return new_type;
}

Type TYPE_BOOL = { .name = "Bool" };
Type TYPE_INT = { .name = "Int" };
Type TYPE_FLOAT = { .name = "Float" };
Type TYPE_RUNE = { .name = "Rune" };
Type TYPE_STR = { .name = "Str" };
Type TYPE_VOID = { .name = "Void" };

static Type **TYPES_BUILTIN = (Type *[]){
	&TYPE_BOOL,
	&TYPE_INT,
	&TYPE_FLOAT,
	&TYPE_RUNE,
	&TYPE_STR,
	&TYPE_VOID,
	NULL
};

/*
Strip underscores from `str`.

This function modifies the contents at `str`.

If `c` is defined, stop iterating when `c` is found.
*/
static void strip_underscore_num(char *str, char c) {
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

bool validate_stmt_type_alias(SemanticState *state, const AstNode *node) {
	const Token *token = node->token;

	if (!node->var_def->is_const) {
		FMT_ERROR(ERR_NO_MUT_TYPE_ALIAS, { .loc = &token->next->location });
		return false;
	}

	const ExprType oper = node->var_def->expr->oper;

	if (oper != EXPR_IDENTIFIER && oper != EXPR_REF) {
		FMT_ERROR(ERR_NO_TYPE_EXPR, { .loc = &token->next->next->location });
		return false;
	}

	const Type *type = token_to_type(state, token->next->next);
	char *alias = token_to_mbs_str(token);

	Symbol *symbol;
	symbol = Calloc(1, sizeof *symbol);
	*symbol = (Symbol){
		.name = alias,
		.location = token->location,
		.expr_type = type,
		.type = SYMBOL_ALIAS,
	};

	if (scope_add_symbol(state, symbol)) {
		return true;
	}

	free(symbol);
	return false;
}

bool is_reference(const Type *type) {
	return !!type->inner;
}

const Type *token_to_type(SemanticState *state, const Token *token) {
	if (is_identifier_like(token)) {
		char *type_name = token_to_mbs_str(token);
		const Type *type = find_type(state, type_name);

		free(type_name);
		return type;
	}

	if (token->type == TOKEN_OPER_REF) {
		return get_reference_type(state, token_to_type(state, token->next));
	}

	return NULL;
}

void free_dynamic_type(HashItem *item) {
	free(((Type *)item->data)->dyn_name);
	free(item->data);
}

bool is_identifier_like(const Token *token) {
	return (
		token->type == TOKEN_DOT_IDENTIFIER ||
		token->type == TOKEN_IDENTIFIER
	);
}
