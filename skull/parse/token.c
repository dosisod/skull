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
} TokenizeCtx;

static bool iter_comment(TokenizeCtx *);
static bool iter_quote(TokenizeCtx *);

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
	Token *const head = make_token();

	TokenizeCtx ctx = {
		.token = head,
		.last = head,
		.code = code,
		.line_num = 1,
		.column = 0
	};

	if (*ctx.code == U'\xFEFF') {
		bool err = false;
		FMT_WARN(err, WARN_NO_BOM, {0});
		if (err) return NULL;

		ctx.code++;
	}

	while (*ctx.code) {
		ctx.column++;

		if (*ctx.code == '#') {

			if (iter_comment(&ctx)) {
				free_tokens(head);
				return NULL;
			}

			ctx.token->type = TOKEN_COMMENT;

			if (!*ctx.code) break;
		}
		else if (is_quote(*ctx.code)) {
			if (iter_quote(&ctx)) {
				free_tokens(head);
				return NULL;
			}
		}
		else if (
			*ctx.code == '{' ||
			*ctx.code == '}' ||
			*ctx.code == '(' ||
			*ctx.code == ')' ||
			*ctx.code == ',' ||
			*ctx.code == '\n'
		) {
			if (ctx.token->begin) {
				ctx.token->end = ctx.code;
				ctx.token = setup_next(ctx.token);
			}

			*ctx.token = (Token){
				.begin = ctx.code,
				.end = ctx.code + 1,
				.location = { .line = ctx.line_num, .column = ctx.column }
			};

			switch (*ctx.code) {
				case '{': ctx.token->type = TOKEN_BRACKET_OPEN; break;
				case '}': ctx.token->type = TOKEN_BRACKET_CLOSE; break;
				case '(': ctx.token->type = TOKEN_PAREN_OPEN; break;
				case ')': ctx.token->type = TOKEN_PAREN_CLOSE; break;
				case ',': ctx.token->type = TOKEN_COMMA; break;
				case '\n': ctx.token->type = TOKEN_NEWLINE; break;
				default: break; // make GCC happy
			}

			ctx.last = ctx.token;
			ctx.token = setup_next(ctx.token);
		}
		else if (!ctx.token->begin) {
			if (!is_whitespace(*ctx.code)) {
				ctx.token->begin = ctx.code;
				ctx.token->location.line = ctx.line_num;
				ctx.token->location.column = ctx.column;
			}
		}
		else if (!ctx.token->end) {
			if (is_whitespace(*ctx.code)) {
				ctx.token->end = ctx.code;
				ctx.last = ctx.token;
				ctx.token = setup_next(ctx.token);
			}
		}

		if (*ctx.code == '\n') {
			ctx.line_num++;
			ctx.column = 0;
		}

		ctx.code++;
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
Iterate through comment.

Return `true` if errors occurred.
*/
static bool iter_comment(TokenizeCtx *ctx) {
	CommentState comment = NO_COMMENT;

	if (ctx->code[1] == ' ' || ctx->code[1] == '\t')
		comment = LINE_COMMENT;

	else if (ctx->code[1] == '{')
		comment = BLOCK_COMMENT;

	else {
		Location location = (Location){
			.line = ctx->line_num,
			.column = ctx->column
		};
		FMT_ERROR(ERR_INVALID_COMMENT_START, { .loc = &location });
		return true;
	}

	if (!ctx->token->begin) {
		ctx->token->begin = ctx->code;
		ctx->token->location.line = ctx->line_num;
		ctx->token->location.column = ctx->column;
	}

	ctx->code++;
	ctx->column++;

	do {
		ctx->code++;
		ctx->column++;

		if (comment == LINE_COMMENT && *ctx->code == '\n') {
			ctx->code--;
			break;
		}
		if (comment == BLOCK_COMMENT && *ctx->code == '#') {
			ctx->code++;
			ctx->column++;

			if (*ctx->code == '}') break;

			if (*ctx->code == '\n') {
				ctx->line_num++;
				ctx->column = 0;
			}

			else if (*ctx->code == '{') {
				Location location = (Location){
					.line = ctx->line_num,
					.column = ctx->column
				};
				FMT_ERROR(ERR_NESTED_BLOCK_COMMENT, { .loc = &location });
				return true;
			}
		}
		else if (*ctx->code == '\n') {
			ctx->line_num++;
			ctx->column = 0;
		}
	} while (*ctx->code);

	if (!*ctx->code && comment == BLOCK_COMMENT) {
		FMT_ERROR(ERR_NO_CLOSING_COMMENT, { .loc = &ctx->token->location });
		return true;
	}

	return false;
}

/*
Iterate through a quote.

Return `true` if errors occurred.
*/
static bool iter_quote(TokenizeCtx *ctx) {
	const char32_t quote = *ctx->code;

	if (!ctx->token->begin) {
		ctx->token->begin = ctx->code;
		ctx->token->location.line = ctx->line_num;
		ctx->token->location.column = ctx->column;
	}

	do {
		ctx->code++;

		if (*ctx->code == '\n') {
			ctx->line_num++;
			ctx->column = 0;
		}
		else if (*ctx->code == '\\' && (
			ctx->code[1] == '\\' ||
			ctx->code[1] == quote
		)) {
			ctx->code++;
			ctx->column++;
		}
		else if (*ctx->code == quote) break;

	} while (*ctx->code);

	if (!*ctx->code) {
		FMT_ERROR(ERR_NO_CLOSING_QUOTE, { .loc = &ctx->token->location });
		return true;
	}

	return false;
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
char32_t *token_to_string(const Token *const token) {
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
char *token_to_mbs_str(const Token *const token) {
	char32_t *const tmp = token_to_string(token);
	char *const ret = c32stombs(tmp, &token->location);

	free(tmp);
	return ret;
}

/*
Returns true if `str` is equal to the value of `token`.
*/
bool token_cmp(const char32_t *const str, const Token *const token) {
	const size_t len = token_len(token);

	return c32slen(str) == len &&
		c32sncmp(str, token->begin, len);
}

/*
Return the string length of `token`.
*/
__attribute__((pure)) size_t token_len(const Token *const token) {
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
