#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "skull/build_data.h"
#include "skull/common/color.h"
#include "skull/common/str.h"
#include "skull/semantic/variable.h"

#include "skull/common/errors.h"


static char *_fmt_message(ErrorType, ErrorCode, Message []);
static void message_stringify(Message *const);
static bool do_show_color(void);

static const char *errors[] = {
	[ERR_UNEXPECTED_TOKEN] = "unexpected token: \"%s\"\n",
	[ERR_EOF_NO_BRACKET] = "Reached EOF, expected closing bracket",
	[ERR_TYPE_MISMATCH] = "type mismatch: expected type \"%s\"\n",
	[ERR_FUNC_TYPE_MISMATCH] = "expected param of type \"%s\", got \"%s\"\n",
	[ERR_EXPECTED_SAME_TYPE] = "expected type \"%s\", got \"%s\"\n",
	[ERR_ASSIGN_BAD_TYPE] = "cannot assign type \"%s\" to type \"%s\"\n",
	[ERR_BAD_ESCAPE] = "bad string escape: \"%s\"\n",
	[ERR_VAR_NOT_FOUND] = "variable \"%s\" not found\n",
	[ERR_OVERFLOW] = "overflow occurred while parsing \"%s\"\n",
	[ERR_MISSING_OPEN_BRAK] = "missing opening bracket",
	[ERR_MISSING_CLOSING_PAREN] = "missing closing parenthesis",
	[ERR_MISSING_DECLARATION] = "function \"%s\" missing declaration\n",
	[ERR_MISSING_BLOCK] = "%s statement must be followed by code block\n",
	[ERR_NO_CLOSING_COMMENT] = "expected closing \"#}\" for block comment",
	[ERR_NO_CLOSING_QUOTE] = "expected closing quote",
	[ERR_NO_VOID_ASSIGN] = \
		"function returning type void cannot be assigned to variable \"%s\"\n",
	[ERR_INVALID_INPUT] = "invalid input: \"%s\"\n",
	[ERR_VAR_ALREADY_DEFINED] = "variable \"%s\" already defined\n",
	[ERR_NON_INT_MAIN] = "returning non-int expression \"%s\" from main\n",
	[ERR_NON_BOOL_EXPR] = "expected boolean expression",
	[ERR_MAIN_RESERVED] = "cannot export function \"main\"",
	[ERR_NO_REDEFINE_FUNC] = "cannot redeclare function \"%s\"\n",
	[ERR_NO_REDEFINE_VAR_AS_FUNC] = \
		"cannot redeclare variable \"%s\" as function\n",
	[ERR_NO_REDEFINE_FUNC_AS_VAR] = \
		"cannot redeclare function \"%s\" as variable\n",
	[ERR_ZERO_PARAM_FUNC] = \
		"passing parameter to function that takes zero parameters",
	[ERR_SHADOW_VAR] = "variable \"%s\" shadows existing variable\n",
	[ERR_EXPECTED_RETURN] = "expected return value in function \"%s\"\n",
	[ERR_NO_VOID_RETURN] = "unexpected return from void function \"%s\"\n",
	[ERR_ELSE_ELIF_MISSING_IF] = \
		"else/elif statement missing preceding if statement",
	[ERR_UNEXPECTED_CODE_BLOCK] = "unexpected code block",
	[ERR_EMPTY_BLOCK] = "code block cannot be empty",
	[ERR_UNREACHABLE_CODE] = "unreachable code",
	[ERR_REASSIGN_CONST] = "cannot reassign const variable \"%s\"\n",
	[ERR_INVALID_EXPR] = "invalid expression near \"%s\"\n",
	[ERR_NO_DANGLING_EXPR] = "expression cannot be used on its own",
	[ERR_REDUNDANT_REASSIGN] = \
		"redundant assignment of variable \"%s\" to itself\n",
	[ERR_DIV_BY_ZERO] = "division by zero",
	[ERR_NO_NESTED] = \
		"cannot declare nested function \"%s\" as external or exported\n",
	[ERR_TOP_LVL_ALIAS_ONLY] = "type aliases must be defined at the top level",
	[ERR_EXPECTED_COMMA] = "expected comma",
	[ERR_UNCLOSED_FUNC_CALL] = "function call missing closing parenthesis",
	[ERR_TYPE_NOT_FOUND] = "type \"%s\" could not be found\n",
	[ERR_ALIAS_ALREADY_DEFINED] = "alias \"%s\" is already defined\n",
	[ERR_ASSIGN_MISSING_EXPR] = "expected expression in assignment variable",
	[ERR_RETURN_MISSING_EXPR] = "expected expression in return",
	[ERR_EXPECTED_EXPR] = "expected expression after \"%s\"\n",
	[ERR_NO_CONTROL_CHAR] = "control character cannot be used in rune",
	[ERR_INVALID_NUM_PARAMS] = "invalid number of parameters",
	[ERR_INVALID_COMMENT_START] = "invalid start of comment",
	[ERR_ILLEGAL_SEQ_AT] = "illegal UTF8 sequence at character offset %s\n",
	[ERR_ILLEGAL_SEQ] = "illegal UTF8 sequence in this region",
	[ERR_POW_BAD_TYPE] = "cannot use type \"%s\" for power operator\n",
	[ERR_EXPECTED_NEWLINE] = "expected a newline",
	[WARN_VAR_NOT_CONST] = "variable \"%s\" should be constant\n",
	[WARN_VAR_UNUSED] = "variable \"%s\" is unused\n",
	[WARN_FUNC_UNUSED] = "function \"%s\" is unused\n",
	[WARN_COND_ALWAYS_TRUE] = "condition is always true",
	[WARN_COND_ALWAYS_FALSE] = "condition is always false",
	[WARN_FILE_EMPTY] = "file is empty",
	[WARN_NESTED_BLOCK_COMMENT] = \
		"cannot put opening block comment in existing block comment",
	[WARN_TRIVIAL_TYPE] = "explicit type \"%s\" can be trivialy deduced\n",
	[ERR_NO_REDEFINE_ALIAS_AS_VAR] = \
		"cannot redeclare type alias \"%s\" as variable\n",
	[ERR_NO_REDEFINE_ALIAS_AS_FUNC] = \
		"cannot redeclare type alias \"%s\" as function\n",
	[ERR_NO_REDEFINE_VAR_AS_ALIAS] = \
		"cannot redeclare variable \"%s\" as type alias\n",
	[ERR_NO_REDEFINE_FUNC_AS_ALIAS] = \
		"cannot redeclare function \"%s\" as type alias\n",
	[ERR_NOT_NUMERIC] = "expected a numeric value"
};

static const unsigned MAX_ERRORS = sizeof(errors) / sizeof(char *);

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

void fmt_message(ErrorType type, ErrorCode id, Message msgs[]) {
	char *msg = _fmt_message(type, id, msgs);

	fprintf(stderr, "%s", msg);
	free(msg);
}

/*
Returns formatted message.

Every `%s` in the string is expanded according to the corresponding `Message`
in `msgs`.
*/
char *_fmt_message(ErrorType type, ErrorCode id, Message msgs[]) {
	Message *msg = msgs;
	message_stringify(msg);

	size_t num_of_percents = 0;
	while (msg->real || msg->str) {
		num_of_percents++;
		msg++;
		message_stringify(msg);
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
			BUILD_DATA.filename
		);
	}
	else if (type == ERROR_WARN) {
		prefix = uvsnprintf(
			do_show_color() ?
				"%s: " COLOR_BOLD COLOR_YELLOW_FG "Warning" COLOR_RESET ": " :
				"%s: Warning: ",
			BUILD_DATA.filename
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

	if (id >= MAX_ERRORS) return NULL;
	const char *fmt = errors[id];

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
Convert error msg `msg` for use in `fmt_message`.

Depending on whether `msg` is a token, a variable, or a string, the resulting
feild `real` will be created differently.

If `i` (integer) is specified for `msg`, it must be one more then the actual
value. This is due to `0` being falsey, and thus not being able to be checked.
*/
void message_stringify(Message *const msg) {
	if (msg->var)
		msg->real = strdup(msg->var->name);
	else if (msg->type)
		msg->real = strdup(msg->type);
	else if (msg->str32)
		msg->real = c32stombs(msg->str32, NULL);
	else if (msg->i)
		msg->real = uvsnprintf("%zu", msg->i - 1);
	else if (msg->tok && !msg->real && !msg->str)
		msg->real = token_to_mbs_str(msg->tok);
}
