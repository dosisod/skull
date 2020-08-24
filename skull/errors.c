#include <stddef.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/eval/variable.h"

#include "skull/errors.h"

#define ERR_CANNOT(do_action) \
	U"cannot " do_action U" type \"%\" with type \"%\""

#define ERR_UNAVAILABLE(operation) \
	U"type \"%\" does not allow for " operation

const char32_t *ERR_UNEXPECTED_TOKEN = U"unexpected token";
const char32_t *ERR_INVALID_INPUT = U"invalid input";
const char32_t *ERR_TYPE_MISMATCH = U"error, type mismatch";
const char32_t *ERR_UNEXPECTED_PARAM = U"\"%\" function does not take any parameters";
const char32_t *ERR_INVALID_PARAMS = U"invalid number of parameters given in function \"%\"";

const char32_t *ERR_VAR_ALREADY_DEFINED = U"variable \"%\" already defined";
const char32_t *ERR_VAR_NOT_FOUND = U"variable \"%\" not found";
const char32_t *ERR_CANNOT_ASSIGN_CONST = U"cannot reassign const variable \"%\"";

const char32_t *ERR_OVERFLOW = U"overflow occurred while parsing \"%\"";
const char32_t *ERR_MISSING_ASSIGNMENT = U"missing value in assignment to variable \"%\"";
const char32_t *ERR_NON_INT_RETURN = U"returning non-int variable \"%\" from main";

const char32_t *ERR_CANNOT_ADD = ERR_CANNOT("add");
const char32_t *ERR_ADD_UNAVAILABLE = ERR_UNAVAILABLE("addition");

const char32_t *ERR_CANNOT_SUB = ERR_CANNOT("subtract");
const char32_t *ERR_SUB_UNAVAILABLE = ERR_UNAVAILABLE("subtraction");

const char32_t *ERR_CANNOT_MULT = ERR_CANNOT("multiply");
const char32_t *ERR_MULT_UNAVAILABLE = ERR_UNAVAILABLE("multiplication");

const char32_t *ERR_CANNOT_DIV = ERR_CANNOT("divide");
const char32_t *ERR_DIV_UNAVAILABLE = ERR_UNAVAILABLE("division");

const char32_t *ERR_TYPE_TYPE_BAD = U"\"type\" cannot be assigned to variable of type \"type\"";

const char32_t *ERR_ASSIGN_FUNC = U"cannot reassign built-in function \"%\"";

#undef ERR_CANNOT
#undef ERR_UNAVAILABLE

typedef struct {
	const char32_t *percent;
	size_t len;
	size_t diff;
} error_chunk_t;

/*
Format an error message.

Every `%` in the string is expanded according to the corresponding `error_msg_t` in `msgs`.

The result of this function must be freed.
*/
char32_t *fmt_error(const char32_t *fmt, error_msg_t msgs[]) {
	const size_t fmt_len = c32slen(fmt);

	error_msg_t *tmp = msgs;
	fmt_error_stringify(tmp);

	size_t num_of_percents = 0;
	while (tmp->real) {
		num_of_percents++;
		tmp++;
		fmt_error_stringify(tmp);
	}

	error_chunk_t *chunks;
	chunks = malloc((num_of_percents + 1) * sizeof *chunks);
	DIE_IF_MALLOC_FAILS(chunks);

	chunks[0].percent = c32schr(fmt, '%');
	if (!chunks[0].percent) {
		free(chunks);
		return NULL;
	}
	chunks[0].diff = (size_t)(chunks[0].percent - fmt);
	chunks[0].len = c32slen(msgs[0].real);

	size_t total_str_len = chunks[0].len;

	size_t at_percent = 1;
	while (at_percent < num_of_percents) {
		chunks[at_percent].percent = c32schr(chunks[at_percent - 1].percent + 1, '%');
		if (!chunks[at_percent].percent) {
			free(chunks);
			return NULL;
		}

		chunks[at_percent].len = c32slen(msgs[at_percent].real);
		total_str_len += chunks[at_percent].len;
		chunks[at_percent].diff = (size_t)(chunks[at_percent].percent - chunks[at_percent - 1].percent - 1);

		at_percent++;
	}
	chunks[at_percent].diff = (size_t)(fmt_len - (unsigned long)(chunks[at_percent - 1].percent - fmt) - 1);

	char32_t *out;
	out = malloc((total_str_len + fmt_len - num_of_percents + 1) * sizeof *out);
	DIE_IF_MALLOC_FAILS(out);

	c32sncpy(out, fmt, chunks[0].diff);
	size_t diffs = chunks[0].diff;
	char32_t *offset = out + chunks[0].diff;
	at_percent = 1;

	while (at_percent <= num_of_percents) {
		c32sncpy(offset, msgs[at_percent - 1].real, chunks[at_percent - 1].len);
		offset += chunks[at_percent - 1].len;
		c32sncpy(offset, fmt + diffs + at_percent, chunks[at_percent].diff);
		diffs += chunks[at_percent].diff;
		offset += chunks[at_percent].diff;
		at_percent++;
	}
	offset[0] = U'\0';

	free(chunks);
	return out;
}

/*
Convert error msg `msg` for use in `fmt_error`.

Depending on whether `msg` is a token, a variable, or a string, the resulting feild `real` will be created differently.
*/
void fmt_error_stringify(error_msg_t *msg) {
	if (msg->tok) {
		msg->real = token_str(msg->tok);
	}
	else if (msg->var) {
		msg->real = c32sdup(msg->var->name);
	}
	else if (msg->type) {
		msg->real = c32sdup(msg->type->name);
	}
	else if (msg->str) {
		msg->real = c32sdup(msg->str);
	}
}

/*
Returns true if `str` is an error msg.
*/
__attribute__((pure)) bool is_error_msg(const char32_t *str) {
	return str && (
		str == ERR_UNEXPECTED_TOKEN ||
		str == ERR_INVALID_INPUT ||
		str == ERR_TYPE_MISMATCH ||
		str == ERR_TYPE_TYPE_BAD ||
		str == ERR_MISSING_TOKEN
	);
}