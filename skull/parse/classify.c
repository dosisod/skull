#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/semantic/types.h"

#include "skull/parse/classify.h"

static bool is_type_str(const char32_t *const);
static bool is_reserved_str(const char32_t *const);
static bool is_constant_integer_str(const char32_t *const);
static bool is_constant_float_str(const char32_t *const);
static bool is_constant_bool_str(const char32_t *const);
static bool is_constant_rune_str(const char32_t *const);
static bool is_constant_str_str(const char32_t *const);

#define KW_MUT U"mut"
#define KW_RETURN U"return"
#define KW_UNREACHABLE U"unreachable"
#define KW_IF U"if"
#define KW_ELIF U"elif"
#define KW_ELSE U"else"
#define KW_WHILE U"while"
#define KW_EXTERNAL U"external"
#define KW_EXPORT U"export"
#define KW_NOOP U"noop"
#define KW_BREAK U"break"
#define KW_CONTINUE U"continue"
#define KW_IMPORT U"import"
#define KW_NAMESPACE U"namespace"
#define KW_MOD U"mod"
#define KW_NOT U"not"
#define KW_IS U"is"
#define KW_ISNT U"isnt"
#define KW_AND U"and"
#define KW_OR U"or"
#define KW_XOR U"xor"
#define KW_INFINITY U"Infinity"
#define KW_NAN U"NaN"
#define KW_TRUE U"true"
#define KW_FALSE U"false"

#define TOKEN_TRY_STR(str, token_type) \
	else if (token_cmp((str), token)) { \
		token->type = (token_type); \
	}

#define TOKEN_SET_IF(cond, token_type) \
	else if ((cond)) { \
		token->type = (token_type); \
	}

/*
Classify the token `token`.
*/
static void classify_token(Token *const token) {
	if (token->type) return;

	const size_t len = token->len;

	if (len == 1) {
		switch (*token->begin) {
			case '=': token->type = TOKEN_OPER_EQUAL; return;
			case '+': token->type = TOKEN_OPER_PLUS; return;
			case '-': token->type = TOKEN_OPER_DASH; return;
			case '*': token->type = TOKEN_OPER_STAR; return;
			case '/': token->type = TOKEN_OPER_DIV; return;
			case '<': token->type = TOKEN_OPER_LESS_THAN; return;
			case '>': token->type = TOKEN_OPER_GTR_THAN; return;
			case '^': token->type = TOKEN_OPER_POW; return;
			default: break;
		}
	}

	char32_t *const str = token_to_string(token);

	if (false) {}

	TOKEN_TRY_STR(KW_MUT, TOKEN_KW_MUT)
	TOKEN_TRY_STR(KW_RETURN, TOKEN_KW_RETURN)
	TOKEN_TRY_STR(KW_UNREACHABLE, TOKEN_KW_UNREACHABLE)
	TOKEN_TRY_STR(KW_IF, TOKEN_KW_IF)
	TOKEN_TRY_STR(KW_ELIF, TOKEN_KW_ELIF)
	TOKEN_TRY_STR(KW_ELSE, TOKEN_KW_ELSE)
	TOKEN_TRY_STR(KW_WHILE, TOKEN_KW_WHILE)
	TOKEN_TRY_STR(KW_EXTERNAL, TOKEN_KW_EXTERNAL)
	TOKEN_TRY_STR(KW_EXPORT, TOKEN_KW_EXPORT)
	TOKEN_TRY_STR(KW_NOOP, TOKEN_KW_NOOP)
	TOKEN_TRY_STR(KW_BREAK, TOKEN_KW_BREAK)
	TOKEN_TRY_STR(KW_CONTINUE, TOKEN_KW_CONTINUE)
	TOKEN_TRY_STR(KW_IMPORT, TOKEN_KW_IMPORT)
	TOKEN_TRY_STR(KW_NAMESPACE, TOKEN_KW_NAMESPACE)
	TOKEN_TRY_STR(KW_MOD, TOKEN_OPER_MOD)
	TOKEN_TRY_STR(U":=", TOKEN_OPER_AUTO_EQUAL)
	TOKEN_TRY_STR(KW_NOT, TOKEN_OPER_NOT)
	TOKEN_TRY_STR(KW_IS, TOKEN_OPER_IS)
	TOKEN_TRY_STR(KW_ISNT, TOKEN_OPER_ISNT)
	TOKEN_TRY_STR(KW_AND, TOKEN_OPER_AND)
	TOKEN_TRY_STR(KW_OR, TOKEN_OPER_OR)
	TOKEN_TRY_STR(KW_XOR, TOKEN_OPER_XOR)
	TOKEN_TRY_STR(U"<=", TOKEN_OPER_LESS_THAN_EQ)
	TOKEN_TRY_STR(U">=", TOKEN_OPER_GTR_THAN_EQ)
	TOKEN_TRY_STR(U"<<", TOKEN_OPER_LSHIFT)
	TOKEN_TRY_STR(U">>", TOKEN_OPER_RSHIFT)

	TOKEN_SET_IF(is_constant_integer_str(str), TOKEN_INT_CONST)
	TOKEN_SET_IF(is_constant_float_str(str), TOKEN_FLOAT_CONST)
	TOKEN_SET_IF(is_constant_bool_str(str), TOKEN_BOOL_CONST)
	TOKEN_SET_IF(is_constant_rune_str(str), TOKEN_RUNE_CONST)
	TOKEN_SET_IF(is_constant_str_str(str), TOKEN_STR_CONST)

	else if (is_valid_identifier_str(str)) {
		token->type = TOKEN_IDENTIFIER;

		bool contains_dot = false;
		if (c32schr(str, '.')) {
			token->type = TOKEN_DOT_IDENTIFIER;
			contains_dot = true;
		}

		if (token->begin[token->len - 1] == ':') {
			if (contains_dot) {
				token->type = TOKEN_UNKNOWN;
			}
			else {
				token->type = TOKEN_NEW_IDENTIFIER;
				str[len - 1] = '\0';

				if (is_reserved_str(str) || is_type_str(str)) {
					token->type = TOKEN_UNKNOWN;
				}
				else {
					token->len--;
				}
			}
		}
	}
	free(str);
}

#undef TOKEN_TRY_STR
#undef TOKEN_SET_IF

/*
Classify all tokens pointed to from `token`.
*/
void classify_tokens(Token *head) {
	while (head) {
		classify_token(head);
		head = head->next;
	}
}

/*
Returns true if `name` is a type string.
*/
static bool is_type_str(const char32_t *const name) {
	char *type_name = c32stombs(name, NULL);

	const bool is_type = find_builtin_type(type_name);
	free(type_name);

	return is_type;
}

/*
Returns true if a `str` is a reserved.
*/
static bool is_reserved_str(const char32_t *const str) {
	const char32_t *reserved[] = {
		KW_MUT, KW_RETURN, KW_UNREACHABLE, KW_IF, KW_ELIF, KW_ELSE, KW_WHILE,
		KW_EXTERNAL, KW_EXPORT, KW_NOOP, KW_BREAK, KW_CONTINUE, KW_IMPORT,
		KW_NAMESPACE, KW_MOD, KW_NOT, KW_IS, KW_ISNT, KW_AND, KW_OR, KW_XOR,
		KW_INFINITY, KW_NAN, KW_TRUE, KW_FALSE, NULL
	};

	const char32_t **at = reserved;

	while (*at) {
		if (c32scmp(*at, str)) return true;

		at++;
	}

	return false;
}

#define EXHAUST_STR_INT(cond) \
	if (*str == '_') return false; \
	bool was_last_underscore = false; \
	while (*str) { \
		if (was_last_underscore && *str == '_') return false; \
		if (!(cond) && *str != '_') return false; \
		was_last_underscore = *str == '_'; \
		str++; \
	} \
	if (str[-1] == '_') return false

#define EXHAUST_STR(cond) \
	while (*str) { \
		if (!(cond)) return false; \
		str++; \
	}

/*
Returns true if `str` is a valid hex/octal/binary/decimal representation
of an integer.

Examples: `-123`, `123`, `0xFF`, `0xff`, `0b1010`, `0o777`
*/
static bool is_constant_integer_str(const char32_t *str) {
	if (*str == '0' && str[1] && !c32isdigit(str[1]) && str[2]) {
		str += 2;

		if (str[-1] == 'x') {
			EXHAUST_STR_INT(c32isxdigit(*str));
		}
		else if (str[-1] == 'b') {
			EXHAUST_STR_INT(*str == '0' || *str == '1');
		}
		else if (str[-1] == 'o') {
			EXHAUST_STR_INT('0' <= *str && *str <= '7');
		}
		else {
			return false;
		}

		return true;
	}

	if (*str == '-') str++;
	if (!*str) return false;

	EXHAUST_STR_INT(c32isdigit(*str));

	return !*str;
}

/*
Returns true if `str` is a valid float (with decimal).

Examples: `123.0`, `-123.0`, `0.0`, `Infinity`
*/
static bool is_constant_float_str(const char32_t *str) {
	if (c32scmp(KW_NAN, str)) return true;
	if (*str == '-') str++;
	if (c32scmp(KW_INFINITY, str)) return true;
	if (*str == '.' || *str == '_') return false;

	bool was_last_underscore = false;
	while (*str && *str != '.') {
		if ((was_last_underscore && *str == '_') ||
			(!c32isdigit(*str) && *str != '_')
		) {
			return false;
		}

		was_last_underscore = *str == '_';
		str++;
	}
	if (str[-1] == '_' || !*str || !str[1]) return false;

	str++;
	EXHAUST_STR(c32isdigit(*str));

	return true;
}

#undef EXHAUST_STR_INT
#undef EXHAUST_STR

/*
Returns true if `str` is a valid bool (`true` or `false`).
*/
static bool is_constant_bool_str(const char32_t *const str) {
	return c32scmp(KW_TRUE, str) || c32scmp(KW_FALSE, str);
}

/*
Returns true if `str` is a valid rune.

Examples: `'x'`, `'\n'`, `'\xFF'`, and `' '`.
Won't work: `''`, `'\'`, `'x '`, or `' x'`.
*/
static bool is_constant_rune_str(const char32_t *const str) {
	const size_t len = c32slen(str);

	if (*str != '\'' || str[len - 1] != '\'')
		return false;

	if (4 <= len && len <= 12)
		return str[1] == '\\';

	return len == 3;
}

/*
Returns true if `str` is a valid string constant.

Examples: `""` and `"hello"`.
*/
static bool is_constant_str_str(const char32_t *const str) {
	const size_t len = c32slen(str);

	return len >= 2 &&
		str[0] == '\"' &&
		str[len - 1] == '\"';
}

/*
Returns true if `str` is a valid identifer.
*/
bool is_valid_identifier_str(const char32_t *str) {
	if (*str < 'A' || ('Z' < *str && *str < 'a') || *str > 'z')
		return false;

	str++;

	while (*str) {
		const char32_t c = *str;

		if (c == ':') {
			str++;
			break;
		}

		if (c == '.') {
			if (!str[1]) return false;
		}
		else if (c != '_' && !c32isalnum(c)) {
			return false;
		}

		str++;
	}

	return !*str;
}
