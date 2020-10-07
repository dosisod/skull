#include <stddef.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/eval/variable.h"

#include "skull/common/errors.h"

const char32_t *ERR_UNEXPECTED_TOKEN = ERR_UNEXPECTED_TOKEN_(U"%");

const char32_t *ERR_EOF_NO_BRACKET = U"Reached EOF, expected closing bracket";

const char32_t *ERR_INVALID_INPUT = ERR_INVALID_INPUT_(U"%");

const char32_t *ERR_TYPE_MISMATCH = ERR_TYPE_MISMATCH_(U"%");

const char32_t *ERR_BAD_ESCAPE = ERR_BAD_ESCAPE_(U"%");

const char32_t *ERR_UNEXPECTED_PARAM = ERR_UNEXPECTED_PARAM_(U"%");

const char32_t *ERR_INVALID_PARAMS = ERR_INVALID_PARAMS_(U"%");

const char32_t *ERR_VAR_ALREADY_DEFINED = ERR_VAR_ALREADY_DEFINED_(U"%");

const char32_t *ERR_VAR_NOT_FOUND = ERR_VAR_NOT_FOUND_(U"%");

const char32_t *ERR_CANNOT_ASSIGN_CONST = ERR_CANNOT_ASSIGN_CONST_(U"%");

const char32_t *ERR_OVERFLOW = ERR_OVERFLOW_(U"%");

const char32_t *ERR_MISSING_ASSIGNMENT = ERR_MISSING_ASSIGNMENT_(U"%");

const char32_t *ERR_NON_INT_RETURN = ERR_NON_INT_RETURN_(U"%");

const char32_t *ERR_CANNOT_ADD = ERR_CANNOT_("add", U"%", U"%");
const char32_t *ERR_ADD_UNAVAILABLE = ERR_UNAVAILABLE_("addition", U"%");

const char32_t *ERR_CANNOT_SUB = ERR_CANNOT_("subtract", U"%", U"%");
const char32_t *ERR_SUB_UNAVAILABLE = ERR_UNAVAILABLE_("subtraction", U"%");

const char32_t *ERR_CANNOT_MULT = ERR_CANNOT_("multiply", U"%", U"%");
const char32_t *ERR_MULT_UNAVAILABLE = ERR_UNAVAILABLE_("multiplication", U"%");

const char32_t *ERR_CANNOT_DIV = ERR_CANNOT_("divide", U"%", U"%");
const char32_t *ERR_DIV_UNAVAILABLE = ERR_UNAVAILABLE_("division", U"%");

const char32_t *ERR_ASSIGN_FUNC = ERR_ASSIGN_FUNC_(U"%");

#undef ERR_CANNOT
#undef ERR_UNAVAILABLE

typedef struct {
	const char32_t *percent;
	size_t len;
	size_t diff;
} ErrorChunk;

/*
Format an error message.

Every `%` in the string is expanded according to the corresponding `ErrorMsg` in `msgs`.

The result of this function must be freed.
*/
char32_t *fmt_error(const char32_t *const fmt, ErrorMsg msgs[]) {
	const size_t fmt_len = c32slen(fmt);

	ErrorMsg *tmp = msgs;
	fmt_error_stringify(tmp);

	size_t num_of_percents = 0;
	while (tmp->real) {
		num_of_percents++;
		tmp++;
		fmt_error_stringify(tmp);
	}
	tmp = msgs;

	ErrorChunk *chunks;
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
	offset[0] = '\0';

	while (tmp->real) {
		free(tmp->real);
		tmp++;
	}

	free(chunks);
	return out;
}

/*
Convert error msg `msg` for use in `fmt_error`.

Depending on whether `msg` is a token, a variable, or a string, the resulting feild `real` will be created differently.
*/
void fmt_error_stringify(ErrorMsg *const msg) {
	if (msg->tok) {
		msg->real = token_str(msg->tok);
	}
	else if (msg->var) {
		msg->real = c32sdup(msg->var->name);
	}
	else if (msg->type) {
		msg->real = mbstoc32s(msg->type->name);
	}
	else if (msg->str) {
		msg->real = c32sdup(msg->str);
	}
}