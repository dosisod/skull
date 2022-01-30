#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "skull/build_data.h"
#include "skull/common/color.h"
#include "skull/common/str.h"
#include "skull/common/vector.h"
#include "skull/semantic/variable.h"

#include "skull/common/errors.h"


static char *fmt_message_internal(ErrorType, ErrorCode, Message []);
static void message_stringify(Message *const);
static void write_error_msg(char *);
static char *get_location_str(const Message *);
static char *get_error_msg(const Message[], size_t, ErrorCode);

const char *color_warning_msg = \
	"%s: " COLOR_BOLD COLOR_YELLOW_FG "Warning" COLOR_RESET ": ";
const char *color_fatal_msg = \
	"%s: " COLOR_BOLD COLOR_RED_FG "Compilation error" COLOR_RESET ": ";
const char *warning_msg = "%s: Warning: ";
const char *fatal_msg = "%s: Compilation error: ";


Vector *error_msgs;

static const char *errors[] = {
	[ERR_UNEXPECTED_TOKEN] = "unexpected token: \"%s\"",
	[ERR_EOF_NO_BRACKET] = "Reached EOF, expected closing bracket",
	[ERR_NOT_INT] = "expected an integer",
	[ERR_FUNC_TYPE_MISMATCH] = "expected param of type \"%s\", got \"%s\"",
	[ERR_EXPECTED_SAME_TYPE] = "expected type \"%s\", got \"%s\"",
	[ERR_RUNE_TOO_LONG] = "rune contains too many characters",
	[ERR_BAD_ESCAPE] = "bad escape sequence: \"%s\"",
	[ERR_VAR_NOT_FOUND] = "variable \"%s\" not found",
	[ERR_OVERFLOW] = "overflow occurred while parsing \"%s\"",
	[ERR_MISSING_OPEN_BRAK] = "missing opening bracket",
	[ERR_MISSING_CLOSING_PAREN] = "missing closing parenthesis",
	[ERR_MISSING_DECLARATION] = "function \"%s\" missing declaration",
	[ERR_MISSING_BLOCK] = "%s statement must be followed by code block",
	[ERR_NO_CLOSING_COMMENT] = "expected closing \"#}\" for block comment",
	[ERR_NO_CLOSING_QUOTE] = "expected closing quote",
	[ERR_NO_VOID_ASSIGN] = \
		"function returning type void cannot be assigned to variable \"%s\"",
	[ERR_IMPORT_IDENT] = "expected identifier after \"import\"",
	[ERR_VAR_ALREADY_DEFINED] = "variable \"%s\" already defined",
	[ERR_NON_INT_MAIN] = "returning non-int expression \"%s\" from main",
	[ERR_NON_BOOL_EXPR] = "expected boolean expression",
	[ERR_MAIN_RESERVED] = "cannot export function \"main\"",
	[ERR_NO_REDEFINE_FUNC] = "cannot redeclare function \"%s\"",
	[ERR_NO_REDEFINE_VAR_AS_FUNC] = \
		"cannot redeclare variable \"%s\" as function",
	[ERR_NO_REDEFINE_FUNC_AS_VAR] = \
		"cannot redeclare function \"%s\" as variable",
	[ERR_ZERO_PARAM_FUNC] = \
		"passing parameter to function that takes zero parameters",
	[ERR_SHADOW_VAR] = "variable \"%s\" shadows existing variable",
	[ERR_EXPECTED_RETURN] = "expected return value in function \"%s\"",
	[ERR_NO_VOID_RETURN] = "unexpected return from void function \"%s\"",
	[ERR_ELSE_ELIF_MISSING_IF] = \
		"else/elif statement missing preceding if statement",
	[ERR_UNEXPECTED_CODE_BLOCK] = "unexpected code block",
	[ERR_EMPTY_BLOCK] = "code block cannot be empty",
	[ERR_UNREACHABLE_CODE] = "unreachable code",
	[ERR_REASSIGN_CONST] = "cannot reassign const variable \"%s\"",
	[ERR_INVALID_EXPR] = "invalid expression near \"%s\"",
	[ERR_NO_DANGLING_EXPR] = "expression cannot be used on its own",
	[ERR_REDUNDANT_REASSIGN] = \
		"redundant assignment of variable \"%s\" to itself",
	[ERR_DIV_BY_ZERO] = "division by zero",
	[ERR_NO_NESTED] = \
		"cannot declare nested function \"%s\" as external or exported",
	[ERR_EXPECTED_COMMA] = "expected comma",
	[ERR_UNCLOSED_FUNC_CALL] = "function call missing closing parenthesis",
	[ERR_TYPE_NOT_FOUND] = "type \"%s\" could not be found",
	[ERR_ALIAS_ALREADY_DEFINED] = "alias \"%s\" is already defined",
	[ERR_ASSIGN_MISSING_EXPR] = "expected expression in assignment variable",
	[ERR_RETURN_MISSING_EXPR] = "expected expression in return",
	[ERR_EXPECTED_EXPR] = "expected expression after \"%s\"",
	[ERR_NO_CONTROL_CHAR] = "control character cannot be used in rune",
	[ERR_INVALID_NUM_PARAMS] = "invalid number of parameters",
	[ERR_INVALID_COMMENT_START] = "invalid start of comment",
	[ERR_ILLEGAL_SEQ_AT] = "illegal UTF8 sequence at character offset %s",
	[ERR_ILLEGAL_SEQ] = "illegal UTF8 sequence in this region",
	[ERR_POW_BAD_TYPE] = "cannot use type \"%s\" for power operator",
	[ERR_EXPECTED_NEWLINE] = "expected a newline",
	[WARN_VAR_NOT_CONST] = "variable \"%s\" should be constant",
	[WARN_VAR_UNUSED] = "variable \"%s\" is unused",
	[WARN_FUNC_UNUSED] = "function \"%s\" is unused",
	[WARN_COND_ALWAYS_TRUE] = "condition is always true",
	[WARN_COND_ALWAYS_FALSE] = "condition is always false",
	[WARN_FILE_EMPTY] = "file is empty",
	[ERR_NESTED_BLOCK_COMMENT] = \
		"cannot put opening block comment in existing block comment",
	[WARN_TRIVIAL_TYPE] = "explicit type \"%s\" can be trivialy deduced",
	[ERR_NO_REDEFINE_ALIAS_AS_VAR] = \
		"cannot redeclare type alias \"%s\" as variable",
	[ERR_NO_REDEFINE_ALIAS_AS_FUNC] = \
		"cannot redeclare type alias \"%s\" as function",
	[ERR_NO_REDEFINE_VAR_AS_ALIAS] = \
		"cannot redeclare variable \"%s\" as type alias",
	[ERR_NO_REDEFINE_FUNC_AS_ALIAS] = \
		"cannot redeclare function \"%s\" as type alias",
	[ERR_NOT_NUMERIC] = "expected a numeric value",
	[WARN_NO_BOM] = "BOM found",
	[ERR_BREAK_NOT_IN_WHILE] = "break must be inside while loop",
	[ERR_CONTINUE_NOT_IN_WHILE] = "continue must be inside while loop",
	[ERR_NO_MUT_TYPE_ALIAS] = "type alias cannot be mutable",
	[ERR_NO_TYPE_EXPR] = "type aliases cannot be used in expressions",
	[ERR_NO_DOUBLE_UNARY] = "cannot use unary operator twice",
	[ERR_UNEXPECTED_UNARY_EXPR] = "unexpected unary operator",
	[ERR_REF_IDENT_ONLY] = "cannot take reference of non-identifier",
	[ERR_MISSING_ASSIGNMENT] = "variable is missing an assignment",
	[ERR_UNEXPECTED_COMMA] = "unexpected comma",
	[ERR_CANNOT_DEREF] = "cannot dereference expression",
};

static const unsigned MAX_ERRORS = sizeof(errors) / sizeof(char *);

void fmt_message(ErrorType type, ErrorCode id, Message msgs[]) {
	char *msg = fmt_message_internal(type, id, msgs);

	if (!error_msgs) error_msgs = make_vector();
	vector_push(error_msgs, msg);
}

/*
Returns formatted message.

Every `%s` in the string is expanded according to the corresponding `Message`
in `msgs`.
*/
static char *fmt_message_internal(
	ErrorType type,
	ErrorCode id,
	Message msgs[]
) {
	if (id >= MAX_ERRORS) return NULL;

	Message *msg = msgs;
	message_stringify(msg);

	size_t num_of_percents = 0;
	while (msg->real || msg->str) {
		num_of_percents++;
		msg++;
		message_stringify(msg);
	}
	msg = msgs;

	char *prefix = uvsnprintf(
		(type == ERROR_FATAL) ?
			(BUILD_DATA.color ?
				color_fatal_msg :
				fatal_msg) :
			(BUILD_DATA.color ?
				color_warning_msg :
				warning_msg),
		BUILD_DATA.filename
	);

	char *location_str = get_location_str(&msg[0]);
	char *error_msg = get_error_msg(msgs, num_of_percents, id);

	char *final_str = uvsnprintf("%s%s%s\n",
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

static char *get_location_str(const Message *msg) {
	const Location *location = msg[0].tok ?
		&msg[0].tok->location :
		(msg[0].loc ?
			msg[0].loc :
			NULL);

	if (!location || !location->line) return NULL;

	return uvsnprintf("line %u column %u: ",
		location->line,
		location->column
	);
}

static char *get_error_msg(
	const Message msgs[],
	size_t num_of_percents, // NOLINT
	ErrorCode id
) {
	if (num_of_percents == 0) return strdup(errors[id]);

	if (num_of_percents == 1) {
		return uvsnprintf(
			errors[id],
			msgs[0].real ? msgs[0].real : msgs[0].str
		);
	}

	if (num_of_percents == 2) {
		return uvsnprintf(
			errors[id],
			msgs[0].real ? msgs[0].real : msgs[0].str,
			msgs[1].real ? msgs[1].real : msgs[1].str
		);
	}

	return NULL;
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
		msg->real = strdup(msg->type->name);
	else if (msg->str32)
		msg->real = c32stombs(msg->str32, NULL);
	else if (msg->i)
		msg->real = uvsnprintf("%zu", msg->i - 1);
	else if (msg->tok && !msg->real && !msg->str)
		msg->real = token_to_mbs_str(msg->tok);
}

void write_and_free_errors(void) {
	if (error_msgs) {
		free_vector(error_msgs, (void(*)(void *))write_error_msg);
		error_msgs = NULL;
	}
}

void free_errors(void) {
	if (error_msgs) {
		free_vector(error_msgs, free);
		error_msgs = NULL;
	}
}

static void write_error_msg(char *msg) {
	if (!BUILD_DATA.quiet) fprintf(stderr, "%s", msg);
	free(msg);
}
