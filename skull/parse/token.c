#include <stdbool.h>

#include "skull/build_data.h"
#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"

#include "skull/parse/token.h"

static bool is_whitespace(char32_t);
static bool is_quote(char32_t);

typedef enum {
	NO_COMMENT,
	LINE_COMMENT,
	BLOCK_COMMENT
} CommentState;

typedef struct {
	Token *token;
	Token *last;
	const char32_t *code;
	unsigned line_num;
	unsigned column;
	CommentState comment;
} TokenizeCtx;

static bool iter_comment(TokenizeCtx *);
static bool iter_quote(TokenizeCtx *);
static bool tokenize_inner_loop(TokenizeCtx *);
static bool is_delimeter(char32_t);
static void token_add_info(TokenizeCtx *);
static void chomp(TokenizeCtx *);
static void next_line(TokenizeCtx *);
static bool is_valid_comment_start(TokenizeCtx *);
static void parse_delimiter(TokenizeCtx *);

/*
Allocate and append next token, return newly created token.
*/
static Token *setup_next(Token *token) {
	return (token->next = make_token());
}

/*
Tokenize `code` into linked list of tokens.
*/
Token *tokenize(const char32_t *code) {
	if (*code == U'\xFEFF') {
		bool err = false;
		FMT_WARN(err, WARN_NO_BOM, {0});
		if (err) return NULL;

		code++;
	}

	Token *head = make_token();

	TokenizeCtx ctx = {
		.token = head,
		.last = head,
		.code = code,
		.line_num = 1
	};

	while (*ctx.code) {
		const bool err = tokenize_inner_loop(&ctx);

		if (err) {
			free_tokens(head);
			return NULL;
		}
	}

	// close dangling token if there was no whitespace at EOF
	if (ctx.token->begin) {
		ctx.token->end = ctx.code;
	}
	// pop last token since it will not have any data
	else if (ctx.token != head) {
		ctx.last->next = NULL;
		free(ctx.token);
	}

	return head;
}

/*
Run one iteration of the main tokenizer function.

Returns `true` if en error occurred.
*/
static bool tokenize_inner_loop(TokenizeCtx *ctx) {
	ctx->column++;

	if (*ctx->code == '#') {
		if (iter_comment(ctx)) return true;

		if (!*ctx->code) return false;
	}
	else if (is_quote(*ctx->code) && iter_quote(ctx)) {
		return true;
	}
	else if (is_delimeter(*ctx->code)) {
		parse_delimiter(ctx);
	}
	else if (!ctx->token->begin) {
		if (!is_whitespace(*ctx->code)) token_add_info(ctx);
	}
	else if (!ctx->token->end && is_whitespace(*ctx->code)) {
		ctx->token->end = ctx->code;
		ctx->last = ctx->token;
		ctx->token = setup_next(ctx->token);
	}

	if (*ctx->code == '\n') next_line(ctx);

	ctx->code++;

	return false;
}

static bool is_delimeter(char32_t c) {
	return (
		c == '{' || c == '}' || c == '(' || c == ')' || c == ',' || c == '\n'
	);
}

/*
Iterate through comment.

Return `true` if errors occurred.
*/
static bool iter_comment(TokenizeCtx *ctx) {
	if (!is_valid_comment_start(ctx)) return true;

	if (!ctx->token->begin) token_add_info(ctx);
	chomp(ctx);

	do {
		chomp(ctx);

		if (ctx->comment == LINE_COMMENT && *ctx->code == '\n') {
			ctx->code--;
			break;
		}
		if (ctx->comment == BLOCK_COMMENT && *ctx->code == '#') {
			chomp(ctx);

			if (*ctx->code == '}') break;

			if (*ctx->code == '\n') next_line(ctx);

			else if (*ctx->code == '{') {
				Location location = (Location){
					.line = ctx->line_num,
					.column = ctx->column
				};
				FMT_ERROR(ERR_NESTED_BLOCK_COMMENT, { .loc = &location });
				return true;
			}
		}
		else if (*ctx->code == '\n') next_line(ctx);

	} while (*ctx->code);

	if (!*ctx->code && ctx->comment == BLOCK_COMMENT) {
		FMT_ERROR(ERR_NO_CLOSING_COMMENT, { .loc = &ctx->token->location });
		return true;
	}

	ctx->token->type = TOKEN_COMMENT;
	return false;
}

static bool is_valid_comment_start(TokenizeCtx *ctx) {
	ctx->comment = NO_COMMENT;

	if (ctx->code[1] == ' ' || ctx->code[1] == '\t')
		ctx->comment = LINE_COMMENT;

	else if (ctx->code[1] == '{')
		ctx->comment = BLOCK_COMMENT;

	else {
		Location location = (Location){
			.line = ctx->line_num,
			.column = ctx->column
		};
		FMT_ERROR(ERR_INVALID_COMMENT_START, { .loc = &location });

		return false;
	}

	return true;
}

/*
Iterate through a quote.

Return `true` if errors occurred.
*/
static bool iter_quote(TokenizeCtx *ctx) {
	const char32_t quote = *ctx->code;

	if (!ctx->token->begin) token_add_info(ctx);

	do {
		ctx->code++;

		if (*ctx->code == '\n') {
			next_line(ctx);
		}
		else if (*ctx->code == '\\' && (
			ctx->code[1] == '\\' ||
			ctx->code[1] == quote
		)) {
			chomp(ctx);
		}
		else if (*ctx->code == quote) break;

	} while (*ctx->code);

	if (!*ctx->code) {
		FMT_ERROR(ERR_NO_CLOSING_QUOTE, { .loc = &ctx->token->location });
		return true;
	}

	return false;
}

static void parse_delimiter(TokenizeCtx *ctx) {
	if (ctx->token->begin) {
		ctx->token->end = ctx->code;
		ctx->token = setup_next(ctx->token);
	}

	*ctx->token = (Token){
		.begin = ctx->code,
		.end = ctx->code + 1,
		.location = { .line = ctx->line_num, .column = ctx->column }
	};

	switch (*ctx->code) {
		case '{': ctx->token->type = TOKEN_BRACKET_OPEN; break;
		case '}': ctx->token->type = TOKEN_BRACKET_CLOSE; break;
		case '(': ctx->token->type = TOKEN_PAREN_OPEN; break;
		case ')': ctx->token->type = TOKEN_PAREN_CLOSE; break;
		case ',': ctx->token->type = TOKEN_COMMA; break;
		case '\n': ctx->token->type = TOKEN_NEWLINE; break;
		default: break; // make GCC happy
	}

	ctx->last = ctx->token;
	ctx->token = setup_next(ctx->token);
}

static void token_add_info(TokenizeCtx *ctx) {
	ctx->token->begin = ctx->code;
	ctx->token->location.line = ctx->line_num;
	ctx->token->location.column = ctx->column;
}

static void chomp(TokenizeCtx *ctx) {
	ctx->column++;
	ctx->code++;
}

static void next_line(TokenizeCtx *ctx) {
	ctx->line_num++;
	ctx->column = 0;
}

/*
Return true if `c` is whitespace.

Whitespace is considered as indent/line related control characters.
*/
static __attribute__((const)) bool is_whitespace(char32_t c) {
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

/*
Return true if `c` is a double or single quote.
*/
static __attribute__((const)) bool is_quote(char32_t c) {
	return c == '\'' || c == '\"';
}

/*
Allocate and return a token with set defaults.
*/
Token *make_token(void) {
	return Calloc(1, sizeof(Token));
}

/*
Make a heap allocated copy of the data inside `token`.

The result of this function must be freed.
*/
char32_t *token_to_string(const Token *token) {
	const size_t len = token_len(token);
	char32_t *str;
	str = Malloc((len + 1) * sizeof *str);

	c32sncpy(str, token->begin, len + 1);
	str[len] = '\0';

	return str;
}

/*
Make a heap allocated copy of the data inside `token` as a multi-byte string.

The result of this function must be freed.
*/
char *token_to_mbs_str(const Token *token) {
	char32_t *tmp = token_to_string(token);
	char *ret = c32stombs(tmp, &token->location);

	free(tmp);
	return ret;
}

/*
Returns true if `str` is equal to the value of `token`.
*/
bool token_cmp(const char32_t *str, const Token *token) {
	const size_t len = token_len(token);

	return c32slen(str) == len && c32sncmp(str, token->begin, len);
}

/*
Return the string length of `token`.
*/
__attribute__((pure)) size_t token_len(const Token *token) {
	return (size_t)(token->end - token->begin);
}

/*
Free all tokens from `head` and beyond.
*/
void free_tokens(Token *head) {
	Token *token;

	while (head) {
		token = head;
		head = head->next;
		token->next = NULL;
		free(token);
	}
}
