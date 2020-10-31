#include <stddef.h>
#include <string.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/eval/variable.h"

#include "skull/common/errors.h"

typedef struct {
	const char *percent;
	size_t len;
	size_t diff;
} ErrorChunk;

/*
Prints formatted an error message.

Every `%s` in the string is expanded according to the corresponding `ErrorMsg` in `msgs`.
*/
void fmt_error(const char *const fmt, ErrorMsg msgs[]) {
	ErrorMsg *msg = msgs;
	fmt_error_stringify(msg);

	size_t num_of_percents = 0;
	while (msg->real) {
		num_of_percents++;
		msg++;
		fmt_error_stringify(msg);
	}
	msg = msgs;

	printf("Compilation error: ");

	if (num_of_percents == 0) {
		printf("%s\n", fmt);
	}
	else if (num_of_percents == 1) {
		printf(fmt, msgs[0].real);
	}
	else if (num_of_percents == 2) {
		printf(fmt, msgs[0].real, msgs[1].real);
	}

	while (msg->real) {
		free(msg->real);
		msg++;
	}
}

/*
Convert error msg `msg` for use in `fmt_error`.

Depending on whether `msg` is a token, a variable, or a string, the resulting feild `real` will be created differently.
*/
void fmt_error_stringify(ErrorMsg *const msg) {
	if (msg->tok) {
		msg->real = token_mbs_str(msg->tok);
	}
	else if (msg->var) {
		msg->real = c32stombs(msg->var->name);
	}
	else if (msg->type) {
		msg->real = strdup(msg->type->name);
	}
	else if (msg->str) {
		msg->real = c32stombs(msg->str);
	}
	else if (msg->_real) {
		msg->real = c32stombs(msg->_real);
	}
}