#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/shared.h"
#include "skull/common/color.h"
#include "skull/common/str.h"
#include "skull/compiler/variable.h"

#include "skull/common/errors.h"

const char *errors[] = {
	"unexpected token: \"%s\"\n",
	"Reached EOF, expected closing bracket",
	"type mismatch: expected type \"%s\"\n",
	"expected param of type \"%s\", got \"%s\"\n",
	"expected type \"%s\", got \"%s\"\n",
	"cannot assign type \"%s\" to type \"%s\"\n",
	"bad string escape: \"%s\"\n",
	"variable \"%s\" not found\n",
	"overflow occurred while parsing \"%s\"\n",
	"missing opening bracket",
	"missing closing parenthesis",
	"function \"%s\" missing declaration\n",
	"%s statement must be followed by code block\n",
	"expected closing \"#}\" for block comment",
	"expected closing quote",
	"function returning type void cannot be assigned to variable \"%s\"\n",
	"invalid input: \"%s\"\n",
	"variable \"%s\" already defined\n",
	"returning non-int expression \"%s\" from main\n",
	"expected boolean expression",
	"cannot export function \"main\"",
	"cannot redeclare function \"%s\"\n",
	"cannot redeclare variable \"%s\" as function\n",
	"cannot redeclare function \"%s\" as variable\n",
	"passing parameter to function that takes zero parameters",
	"variable \"%s\" shadows existing variable\n",
	"expected return value in function \"%s\"\n",
	"unexpected return from void function \"%s\"\n",
	"else statement missing preceding if statement",
	"unexpected code block",
	"code block cannot be empty",
	"unreachable code",
	"cannot reassign const variable \"%s\"\n",
	"invalid expression near \"%s\"\n",
	"expression cannot be used on its own",
	"redundant assignment of variable \"%s\" to itself\n",
	"division by zero",
	"cannot declare nested function \"%s\" as external or exported\n",
	"type aliases must be defined at the top level",
	"cannot compare value near \"%s\"\n",
	"expected comma",
	"function call missing closing parenthesis",
	"type \"%s\" could not be found\n",
	"alias \"%s\" is already defined\n",
	"expected expression in assignment variable",
	"expected expression in return",
	"expected expression after \"%s\"\n",
	"control character cannot be used in rune",
	"invalid number of parameters",
	"invalid start of comment",
	"illegal UTF8 sequence at character offset %s\n",
	"illegal UTF8 sequence in this region",
	"cannot use type \"%s\" for power operator\n",
	"expected a newline",
	"variable \"%s\" should be constant\n",
	"variable \"%s\" is unused\n",
	"function \"%s\" is unused\n",
	"condition is always true",
	"condition is always false",
	"file is empty",
	"cannot put opening block comment in existing block comment",
	"explicit type \"%s\" can be trivialy deduced\n"
};

const unsigned MAX_ERRORS = sizeof(errors) / sizeof(char *);

/*
Return whether color output should be displayed.
*/
static bool do_show_color(void) {
	static int flag;

	if (!flag) {
		const char *use_color = getenv("COLOR");

		flag = (use_color && *use_color == '1') ? 2 : 1;
	}

	return flag - 1;
}


/*
Returns formatted error message.

Every `%s` in the string is expanded according to the corresponding `ErrorMsg`
in `msgs`.
*/
char *fmt_error(ErrorType type, ErrorCode error_code, ErrorMsg msgs[]) {
	ErrorMsg *msg = msgs;
	fmt_error_stringify(msg);

	size_t num_of_percents = 0;
	while (msg->real || msg->str) {
		num_of_percents++;
		msg++;
		fmt_error_stringify(msg);
	}
	msg = msgs;

	char *prefix = NULL;
	if (type == ERROR_FATAL) {
		prefix = uvsnprintf(
			do_show_color() ? (
					"%s: "
					COLOR_BOLD COLOR_RED_FG
					"Compilation error"
					COLOR_RESET ": "
				) :
				"%s: Compilation error: ",
			SKULL_STATE.filename
		);
	}
	else if (type == ERROR_WARN) {
		prefix = uvsnprintf(
			do_show_color() ?
				"%s: " COLOR_BOLD COLOR_YELLOW_FG "Warning" COLOR_RESET ": " :
				"%s: Warning: ",
			SKULL_STATE.filename
		);
	}

	const Location *location = msg[0].tok ?
		&msg[0].tok->location :
		(msg[0].loc ?
			msg[0].loc :
			NULL);

	// if location is invalid, dont use it in error
	if (location && !location->line) location = NULL;

	char *location_str = NULL;
	if (location) {
		location_str = uvsnprintf("line %u column %u: ",
			location->line,
			location->column
		);
	}

	if (error_code < 1000 || (error_code - 1000) >= MAX_ERRORS) return NULL;
	const char *fmt = errors[error_code - 1000];

	char *error_msg = NULL;
	if (num_of_percents == 0)
		error_msg = uvsnprintf("%s\n", fmt);
	else if (num_of_percents == 1)
		error_msg = uvsnprintf(
			fmt,
			msgs[0].real ? msgs[0].real : msgs[0].str
		);
	else if (num_of_percents == 2)
		error_msg = uvsnprintf(
			fmt,
			msgs[0].real ? msgs[0].real : msgs[0].str,
			msgs[1].real ? msgs[1].real : msgs[1].str
		);

	char *final_str = uvsnprintf("%s%s%s",
		prefix,
		location_str ? location_str : "",
		error_msg
	);
	free(prefix);
	free(location_str);
	free(error_msg);

	while (msg->real || msg->str) {
		free(msg->real);
		msg++;
	}

	return final_str;
}

/*
Convert error msg `msg` for use in `fmt_error`.

Depending on whether `msg` is a token, a variable, or a string, the resulting
feild `real` will be created differently.

If `i` (integer) is specified for `msg`, it must be one more then the actual
value. This is due to `0` being falsey, and thus not being able to be checked.
*/
void fmt_error_stringify(ErrorMsg *const msg) {
	if (msg->var)
		msg->real = strdup(msg->var->name);
	else if (msg->type)
		msg->real = strdup(msg->type);
	else if (msg->str32)
		msg->real = c32stombs(msg->str32, NULL);
	else if (msg->i)
		msg->real = uvsnprintf("%zu", msg->i - 1);
	else if (msg->tok && !msg->real && !msg->str)
		msg->real = token_mbs_str(msg->tok);
}
