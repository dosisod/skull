#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/shared.h"
#include "skull/common/color.h"
#include "skull/common/str.h"
#include "skull/compiler/variable.h"

#include "skull/common/errors.h"

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
char *fmt_error(ErrorType type, const char *const fmt, ErrorMsg msgs[]) {
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

#ifdef __clang__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wformat-nonliteral"
#else
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif

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

#ifdef __clang__
# pragma clang diagnostic pop
#else
# pragma GCC diagnostic pop
#endif

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
