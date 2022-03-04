#include <stdbool.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/range.h"
#include "skull/common/str.h"
#include "skull/semantic/symbol.h"
#include "skull/semantic/variable.h"

#include "skull/parse/ast_node.h"

typedef struct {
	AstNode *node;
	AstNode *head;
	Token *token;
	unsigned indent_lvl;
	bool err;
} ParserCtx;

static AstNodeExpr *parse_expression(ParserCtx *);
static AstNodeExpr *parse_func_call(ParserCtx *);
static AstNode *parse_ast_tree_(ParserCtx *);
static void free_ast_function_proto(AstNode *);
static void free_ast_var_def(AstNode *);
static void free_ast_var_assign(AstNode *);
static void free_ast_node(AstNode *);
static bool is_single_token_expr(TokenType);
static void free_expr_node(AstNodeExpr *);
static AstNode *push_ast_node(ParserCtx *, Token *, NodeType);
static void print_ast_tree_(const AstNode *, unsigned);
static AstNodeExpr *parse_single_token_expr(ParserCtx *);
static AstNodeExpr *parse_paren_expr(ParserCtx *);
static void parse_ast_sub_tree_(ParserCtx *);
static bool parse_ast_node(ParserCtx *);
static void next_token(ParserCtx *);
static AstNodeExpr *parse_single_expr(ParserCtx *);
static AstNodeExpr *parse_expr_rhs(ParserCtx *, AstNodeExpr *);
static unsigned oper_to_precedence(ExprType);
static AstNodeExpr *parse_root_expr(ParserCtx *);
static bool is_unary_oper(ExprType);
static ExprType get_potential_unary_oper(TokenType);
static bool is_explicit_type(ParserCtx *);
static Token *is_potential_type(Token *);
static Vector *parse_function_proto_params(ParserCtx *);


typedef enum {
	RESULT_OK,
	RESULT_IGNORE,
	RESULT_ERROR
} ParserResult;

#define AST_TOKEN_CMP2(tok, type1, type2) \
	((tok) && (tok)->type == (type1) && \
	(tok)->next && (tok)->next->type == (type2))

/*
Makes an AST (abstract syntax tree) from a list of tokens.
*/
AstNode *parse_ast_tree(Token *token) {
	Token *head = token;

	ParserCtx ctx = { .token = token };

	AstNode *tree = parse_ast_tree_(&ctx);

	if (tree && !tree->token) {
		// in case the first node does not have a token, assign the head of
		// the token list to it
		tree->token = head;
	}

	if (ctx.err) {
		free_ast_tree(tree);
		return NULL;
	}

	return tree;
}

static void parse_return(ParserCtx *ctx) {
	AstNode *return_node = push_ast_node(ctx, ctx->token, AST_NODE_RETURN);
	next_token(ctx);

	AstNodeExpr *expr = parse_expression(ctx);
	if (ctx->err) return;

	if (expr) return_node->expr = expr;
}

static ParserResult parse_var_def(ParserCtx *ctx) {
	bool is_const = true;
	bool is_implicit = true;
	bool is_exported = false;
	Token *first = ctx->token;

	if (ctx->token->type == TOKEN_KW_MUT) {
		is_const = false;
		next_token(ctx);
	}
	else if (ctx->token->type == TOKEN_KW_EXPORT) {
		is_exported = true;
		next_token(ctx);
	}

	if (!ctx->token) {
		ctx->token = first;
		return RESULT_IGNORE;
	}

	Token *name_token = ctx->token;

	if (is_explicit_type(ctx)) {
		is_implicit = false;

		if (!ctx->token) {
			FMT_ERROR(ERR_MISSING_ASSIGNMENT, { .loc = &name_token->location });
			return RESULT_ERROR;
		}
	}
	else if (AST_TOKEN_CMP2(ctx->token,
		TOKEN_IDENTIFIER,
		TOKEN_OPER_AUTO_EQUAL)
	) {
		next_token(ctx);
	}
	else {
		ctx->token = first;
		return RESULT_IGNORE;
	}

	AstNode *var_def_node = push_ast_node(ctx, first, AST_NODE_VAR_DEF);
	next_token(ctx);

	AstNodeExpr *expr = parse_expression(ctx);
	if (ctx->err) return RESULT_ERROR;

	if (!expr) {
		FMT_ERROR(ERR_ASSIGN_MISSING_EXPR, { .loc = &ctx->token->location });
		return RESULT_ERROR;
	}

	var_def_node->var_def = Malloc(sizeof(AstNodeVarDef));
	*var_def_node->var_def = (AstNodeVarDef){
		.is_const = is_const,
		.is_implicit = is_implicit,
		.is_exported = is_exported,
		.name_tok = name_token,
		.expr = expr
	};

	if (ctx->token && ctx->token->type != TOKEN_NEWLINE) {
		FMT_ERROR(ERR_EXPECTED_NEWLINE, {
			.loc = &ctx->token->location
		});
		return RESULT_ERROR;
	}

	return RESULT_OK;
}

static bool is_explicit_type(ParserCtx *ctx) {
	Token *head = ctx->token;

	if (!head) return false;

	if (ctx->token->type != TOKEN_NEW_IDENTIFIER || !ctx->token->next) {
		return false;
	}

	next_token(ctx);

	Token *type_token = is_potential_type(ctx->token);
	if (type_token) {
		ctx->token = type_token->next;
		return true;
	}

	ctx->token = head;
	return false;
}

/*
Check whether the tokens starting at `token` is a potential type. Return
last token of type if valid, otherwise `NULL`.
*/
static Token *is_potential_type(Token *token) {
	if (!token) return NULL;

	if (token->type == TOKEN_IDENTIFIER) {
		return token;
	}

	if (token->type == TOKEN_OPER_REF) {
		return is_potential_type(token->next);
	}

	return NULL;
}

static ParserResult parse_var_assign(ParserCtx *ctx) {
	if (!AST_TOKEN_CMP2(ctx->token, TOKEN_IDENTIFIER, TOKEN_OPER_EQUAL)) {
		return RESULT_IGNORE;
	}

	Token *last = ctx->token;
	next_token(ctx);

	AstNode *var_assign_node = push_ast_node(ctx, last, AST_NODE_VAR_ASSIGN);
	next_token(ctx);

	AstNodeExpr *expr = parse_expression(ctx);
	if (!expr) {
		FMT_ERROR(ERR_ASSIGN_MISSING_EXPR, { .loc = &ctx->token->location });

		return RESULT_ERROR;
	}

	var_assign_node->var_assign = Calloc(1, sizeof(AstNodeVarAssign));
	var_assign_node->var_assign->expr = expr;

	return RESULT_OK;
}

/*
Convert a `TokenType` to an `ExprType` in a binary-expr context
*/
static ExprType token_type_to_binary_expr_type(TokenType type) {
	switch (type) {
		case TOKEN_OPER_PLUS: return EXPR_ADD;
		case TOKEN_OPER_DASH: return EXPR_SUB;
		case TOKEN_OPER_STAR: return EXPR_MULT;
		case TOKEN_OPER_DIV: return EXPR_DIV;
		case TOKEN_OPER_MOD: return EXPR_MOD;
		case TOKEN_OPER_LSHIFT: return EXPR_LSHIFT;
		case TOKEN_OPER_RSHIFT: return EXPR_RSHIFT;
		case TOKEN_OPER_POW: return EXPR_POW;
		case TOKEN_OPER_IS: return EXPR_IS;
		case TOKEN_OPER_ISNT: return EXPR_ISNT;
		case TOKEN_OPER_LESS_THAN: return EXPR_LESS_THAN;
		case TOKEN_OPER_GTR_THAN: return EXPR_GTR_THAN;
		case TOKEN_OPER_LESS_THAN_EQ: return EXPR_LESS_THAN_EQ;
		case TOKEN_OPER_GTR_THAN_EQ: return EXPR_GTR_THAN_EQ;
		case TOKEN_OPER_AND: return EXPR_AND;
		case TOKEN_OPER_XOR: return EXPR_XOR;
		case TOKEN_OPER_OR: return EXPR_OR;
		case TOKEN_OPER_NOT: return EXPR_NOT;
		case TOKEN_OPER_REF: return EXPR_REF;
		default: return EXPR_UNKNOWN;
	}
}

static void free_param(AstNodeFunctionParam *param) {
	if (!param) return;

	if (param->symbol && param->symbol->var && !param->symbol->var->name) {
		free(param->symbol->var);
	}
	free(param->param_name);
	free(param);
}

static ParserResult parse_function_proto(ParserCtx *ctx) {
	Token *first = ctx->token;

	const bool is_external = ctx->token->type == TOKEN_KW_EXTERNAL;
	const bool is_export = ctx->token->type == TOKEN_KW_EXPORT;

	if (is_external || is_export) next_token(ctx);

	if (!AST_TOKEN_CMP2(ctx->token,
		TOKEN_IDENTIFIER,
		TOKEN_PAREN_OPEN)
	) {
		ctx->token = first;
		return RESULT_IGNORE;
	}

	const Token *func_name_token = ctx->token;
	ctx->token = ctx->token->next->next;

	Token *last_token = ctx->token;
	Vector *params = parse_function_proto_params(ctx);

	if (!ctx->token) {
		FMT_ERROR(ERR_MISSING_CLOSING_PAREN, {
			.loc = &func_name_token->next->location
		});

		free_vector(params, (void(*)(void *))free_param);
		return RESULT_ERROR;
	}

	const bool is_proto = ctx->token != last_token;

	const TokenType token_type = is_external ?
		TOKEN_NEWLINE :
		TOKEN_BRACKET_OPEN;

	Token *tmp = NULL;
	Token *return_type_token = NULL;

	if (ctx->token->type == TOKEN_PAREN_CLOSE &&
		ctx->token->next &&
		(tmp = is_potential_type(ctx->token->next))
	) {
		if (tmp->next && tmp->next->type == token_type) {
			return_type_token = ctx->token->next;
			ctx->token = tmp;
		}
		else {
			FMT_ERROR(
				token_type == TOKEN_NEWLINE ?
					ERR_EXPECTED_NEWLINE :
					ERR_MISSING_OPEN_BRAK,
				{ .loc = &tmp->location }
			);

			free_vector(params, (void(*)(void *))free_param);
			return RESULT_ERROR;
		}
	}

	else if (!AST_TOKEN_CMP2(ctx->token, TOKEN_PAREN_CLOSE, token_type)) {
		free_vector(params, (void(*)(void *))free_param);

		if (is_proto) {
			FMT_ERROR(ERR_EXPECTED_COMMA, {
				.loc = &ctx->token->location
			});

			return RESULT_ERROR;
		}

		if (token_type == TOKEN_NEWLINE && !ctx->token->next) {
			FMT_ERROR(ERR_EXPECTED_NEWLINE, { .loc = &ctx->token->location });

			return RESULT_ERROR;
		}

		ctx->token = first;
		return RESULT_IGNORE;
	}

	const unsigned short num_params = (unsigned short)params->length;

	ctx->node->func_proto = Malloc(
		sizeof(AstNodeFunctionProto) +
		(num_params * sizeof(AstNodeFunctionParam *))
	);
	*ctx->node->func_proto = (AstNodeFunctionProto){
		.name_tok = func_name_token,
		.return_type_token = return_type_token,
		.is_external = is_external,
		.is_export = is_export,
		.num_params = num_params
	};

	if (num_params) {
		memcpy(
			&ctx->node->func_proto->params,
			vector_freeze(params),
			(num_params * sizeof(AstNodeFunctionParam *))
		);

		// since the array is frozen, the elements will not be allowed
		// to be freed, so manually free them.
		free(params->elements);
	}

	free_vector(params, NULL);

	push_ast_node(ctx, first, AST_NODE_FUNCTION_PROTO);
	next_token(ctx);

	return RESULT_OK;
}

static Vector *parse_function_proto_params(ParserCtx *ctx) {
	Vector *params = make_vector();
	Token *last_token = ctx->token;

	while (is_explicit_type(ctx)) {
		if (!ctx->token) return params;

		Token *last = ctx->token;

		ctx->token = last_token;

		AstNodeFunctionParam *param;
		param = Malloc(sizeof *param);
		param->type_name = ctx->token->next;
		param->param_name = token_to_string(ctx->token);
		param->location = &ctx->token->location;
		param->symbol = NULL;

		vector_push(params, param);

		ctx->token = last;
		last_token = last->next;
		if (ctx->token->type != TOKEN_COMMA) break;
		next_token(ctx);
	}

	return params;
}

static void parse_condition(ParserCtx *ctx, NodeType node_type) {
	if (!ctx->token || !ctx->token->next) {
		ctx->err = true;
		return;
	}

	AstNode *cond_node = push_ast_node(ctx, ctx->token, node_type);
	next_token(ctx);

	AstNodeExpr *expr = parse_expression(ctx);

	if (!expr || ctx->err) {
		FMT_ERROR(ERR_INVALID_EXPR, { .tok = ctx->token });
		ctx->err = true;

		return;
	}

	cond_node->expr = expr;
}

static void parse_if(ParserCtx *ctx) {
	parse_condition(ctx, AST_NODE_IF);
}

static void parse_elif(ParserCtx *ctx) {
	parse_condition(ctx, AST_NODE_ELIF);
}

static void parse_else(ParserCtx *ctx) {
	push_ast_node(ctx, ctx->token, AST_NODE_ELSE);
	next_token(ctx);
}

static void parse_while(ParserCtx *ctx) {
	parse_condition(ctx, AST_NODE_WHILE);
}

/*
Internal AST tree generator.

Do not set the `node` or `head` fields in `ctx`, they will be overridden.
*/
static AstNode *parse_ast_tree_(ParserCtx *ctx) {
	ctx->node = make_ast_node();
	ctx->head = ctx->node;

	while (ctx->token) {
		const bool done = parse_ast_node(ctx);

		if (done || ctx->err) break;
	}

	if (ctx->err) {
		free_ast_tree(ctx->head);
		return NULL;
	}

	if (!ctx->token && ctx->indent_lvl != 0) {
		FMT_ERROR(ERR_EOF_NO_BRACKET, {0});

		free_ast_tree(ctx->head);
		ctx->err = true;
		return NULL;
	}

	if (ctx->node->last && ctx->head != ctx->node) {
		ctx->node->last->next = NULL;
		ctx->node->last = NULL;
		free(ctx->node);
	}

	return ctx->head;
}

static void parse_unreachable(ParserCtx *ctx) {
	push_ast_node(ctx, ctx->token, AST_NODE_UNREACHABLE);
	next_token(ctx);
}

static void parse_comment(ParserCtx *ctx) {
	push_ast_node(ctx, ctx->token, AST_NODE_COMMENT);
	next_token(ctx);
}

static void parse_noop(ParserCtx *ctx) {
	push_ast_node(ctx, ctx->token, AST_NODE_NOOP);
	next_token(ctx);
}

static void parse_break(ParserCtx *ctx) {
	push_ast_node(ctx, ctx->token, AST_NODE_BREAK);
	next_token(ctx);
}

static void parse_continue(ParserCtx *ctx) {
	push_ast_node(ctx, ctx->token, AST_NODE_CONTINUE);
	next_token(ctx);
}

static void parse_import(ParserCtx *ctx) {
	const Token *next = ctx->token->next;

	if (next && next->type == TOKEN_IDENTIFIER) {
		push_ast_node(ctx, ctx->token, AST_NODE_IMPORT);

		ctx->token = next->next;
	}
	else {
		FMT_ERROR(ERR_IMPORT_IDENT, {
			.loc = next ? &next->location : &ctx->token->location
		});

		ctx->err = true;
	}
}

/*
Parse a single AST node.

Return `true` if a terminating token was reached (closing bracket).
*/
static bool parse_ast_node(ParserCtx *ctx) {
	switch (ctx->token->type) {
		case TOKEN_BRACKET_CLOSE: {
			if (ctx->indent_lvl == 0) {
				FMT_ERROR(ERR_MISSING_OPEN_BRAK, {
					.loc = &(ctx->token)->location
				});

				ctx->err = true;
			}
			return true;
		}
		case TOKEN_BRACKET_OPEN: parse_ast_sub_tree_(ctx); break;
		case TOKEN_NEWLINE:
		case TOKEN_COMMA: next_token(ctx); break;
		case TOKEN_KW_RETURN: parse_return(ctx); break;
		case TOKEN_KW_IF: parse_if(ctx); break;
		case TOKEN_KW_ELIF: parse_elif(ctx); break;
		case TOKEN_KW_ELSE: parse_else(ctx); break;
		case TOKEN_KW_WHILE: parse_while(ctx); break;
		case TOKEN_KW_UNREACHABLE: parse_unreachable(ctx); break;
		case TOKEN_COMMENT: parse_comment(ctx); break;
		case TOKEN_KW_NOOP: parse_noop(ctx); break;
		case TOKEN_KW_BREAK: parse_break(ctx); break;
		case TOKEN_KW_CONTINUE: parse_continue(ctx); break;
		case TOKEN_KW_IMPORT: parse_import(ctx); break;
		default: {
			ParserResult result = parse_function_proto(ctx);

			if (result == RESULT_IGNORE)
				result = parse_var_def(ctx);

			if (result == RESULT_IGNORE)
				result = parse_var_assign(ctx);

			if (result == RESULT_IGNORE) {
				Token *last = ctx->token;
				AstNodeExpr *expr = parse_expression(ctx);

				if (expr) {
					AstNode *expr_node = push_ast_node(
						ctx, last, AST_NODE_EXPR
					);

					expr_node->expr = expr;
				}
				else {
					result = RESULT_ERROR;

					if (!ctx->err) FMT_ERROR(ERR_UNEXPECTED_TOKEN, {
						.tok = ctx->token
					});
				}
			}

			if (result == RESULT_ERROR) ctx->err = true;

			return false;
		}
	}

	return false;
}

/*
Start parsing a AST sub tree.
*/
static void parse_ast_sub_tree_(ParserCtx *ctx) {
	if (ctx->node->last && ctx->node->last->child) {
		FMT_ERROR(ERR_UNEXPECTED_CODE_BLOCK, {
			.loc = &(ctx->token)->location
		});

		ctx->err = true;
		return;
	}
	next_token(ctx);

	ParserCtx new_ctx = {
		.token = ctx->token,
		.indent_lvl = ctx->indent_lvl + 1
	};

	AstNode *child = parse_ast_tree_(&new_ctx);
	ctx->token = new_ctx.token;

	if (!child || new_ctx.err) {
		ctx->err = true;
		return;
	}

	if (!child->token) {
		FMT_ERROR(ERR_EMPTY_BLOCK, { .loc = &(ctx->token)->location });

		free_ast_tree(child);
		ctx->err = true;
		return;
	}
	if (!ctx->node->last) {
		ctx->head->child = child;
		child->parent = ctx->head;
	}
	else {
		ctx->node->last->child = child;
		child->parent = ctx->node->last;
	}

	next_token(ctx);
}

/*
Try and generate AST node for expression.

Returns node if one was added, NULL otherwise.
*/
static AstNodeExpr *parse_expression(ParserCtx *ctx) {
	if (!ctx->token) return NULL;

	AstNodeExpr *head = parse_single_expr(ctx);
	if (ctx->err) return NULL;

	AstNodeExpr *current = head;
	AstNodeExpr *last = NULL;

	while (last != current) {
		last = current;

		current = parse_expr_rhs(ctx, current);
		if (ctx->err) {
			free_expr_node(head);
			return NULL;
		}

		const unsigned last_precedence = oper_to_precedence(last->oper);
		const unsigned current_precedence = oper_to_precedence(current->oper);

		if (last_precedence > current_precedence) {
			current->lhs.expr = last->rhs;
			last->rhs = current;

			AstNodeExpr *tmp = last;
			last = current;
			current = tmp;
		}
	}

	return last ? last : current;
}

static AstNodeExpr *parse_single_expr(ParserCtx *ctx) {
	if (!ctx->token) return NULL;

	ExprType unary_oper = get_potential_unary_oper(ctx->token->type);
	if (!unary_oper) {
		return parse_root_expr(ctx);
	}

	Token *last = ctx->token;
	next_token(ctx);

	if (!ctx->token) {
		FMT_ERROR(ERR_EXPECTED_EXPR, { .tok = last });

		ctx->err = true;
		return NULL;
	}

	if (get_potential_unary_oper(ctx->token->type)) {
		FMT_ERROR(ERR_NO_DOUBLE_UNARY, { .loc = &ctx->token->location });

		ctx->err = true;
		return NULL;
	}

	AstNodeExpr *expr = Malloc(sizeof(AstNodeExpr));
	*expr = (AstNodeExpr){
		.oper = unary_oper,
		.rhs = parse_root_expr(ctx)
	};

	return expr;
}

static bool is_unary_oper(ExprType oper) {
	return (
		oper == EXPR_UNARY_NEG ||
		oper == EXPR_NOT ||
		oper == EXPR_REF ||
		oper == EXPR_DEREF
	);
}

static ExprType get_potential_unary_oper(TokenType type) {
	switch (type) {
		case TOKEN_OPER_DASH: return EXPR_UNARY_NEG;
		case TOKEN_OPER_STAR: return EXPR_DEREF;
		case TOKEN_OPER_NOT: return EXPR_NOT;
		case TOKEN_OPER_REF: return EXPR_REF;
		default: return EXPR_UNKNOWN;
	}
}

/*
Parse root expression from context `ctx`. A root expression is an expression
which has no association, and as such, has the highest precedence.
*/
static AstNodeExpr *parse_root_expr(ParserCtx *ctx) {
	if (ctx->token->type == TOKEN_PAREN_OPEN) {
		return parse_paren_expr(ctx);
	}
	if (AST_TOKEN_CMP2(ctx->token, TOKEN_IDENTIFIER, TOKEN_PAREN_OPEN)) {
		return parse_func_call(ctx);
	}
	if (is_single_token_expr(ctx->token->type)) {
		return parse_single_token_expr(ctx);
	}

	return NULL;
}

static AstNodeExpr *parse_expr_rhs(ParserCtx *ctx, AstNodeExpr *expr) {
	if (!ctx->token) return expr;

	const ExprType oper = token_type_to_binary_expr_type(ctx->token->type);

	if (oper == EXPR_UNKNOWN) return expr;
	if (is_unary_oper(oper)) {
		FMT_ERROR(ERR_UNEXPECTED_UNARY_EXPR, { .loc = &ctx->token->location });
		ctx->err = true;
		return NULL;
	}

	Token *oper_token = ctx->token;
	next_token(ctx);

	AstNodeExpr *rhs = parse_single_expr(ctx);
	if (!rhs) {
		FMT_ERROR(ERR_EXPECTED_EXPR, { .tok = oper_token });

		ctx->err = true;
		return expr;
	}

	AstNodeExpr *new_expr = Malloc(sizeof(AstNodeExpr));
	*new_expr = (AstNodeExpr){
		.lhs = { .expr = expr },
		.oper = oper,
		.rhs = rhs
	};

	return new_expr;
}

static unsigned oper_to_precedence(ExprType oper) {
	switch (oper) {
		case EXPR_UNKNOWN:
			return 0;
		case EXPR_IDENTIFIER:
		case EXPR_CONST:
		case EXPR_FUNC:
			return 1;
		case EXPR_POW:
			return 2;
		case EXPR_NOT:
		case EXPR_UNARY_NEG:
		case EXPR_DEREF:
			return 3;
		case EXPR_MULT:
		case EXPR_DIV:
		case EXPR_MOD:
			return 4;
		case EXPR_ADD:
		case EXPR_SUB:
			return 5;
		case EXPR_AND:
		case EXPR_OR:
		case EXPR_XOR:
		case EXPR_LSHIFT:
		case EXPR_RSHIFT:
			return 6;
		case EXPR_IS:
		case EXPR_ISNT:
		case EXPR_LESS_THAN:
		case EXPR_GTR_THAN:
		case EXPR_LESS_THAN_EQ:
		case EXPR_GTR_THAN_EQ:
			return 7;
		default: return 0;
	}
}

static AstNodeExpr *parse_paren_expr(ParserCtx *ctx) {
	const Token *old_token = ctx->token;

	next_token(ctx);

	AstNodeExpr *expr = parse_expression(ctx);

	if (expr && (!ctx->token || ctx->token->type != TOKEN_PAREN_CLOSE)) {
		FMT_ERROR(ERR_MISSING_CLOSING_PAREN, { .loc = &old_token->location });

		free_expr_node(expr);
		ctx->err = true;
		return NULL;
	}

	if (!expr) {
		FMT_ERROR(ERR_INVALID_EXPR, { .tok = old_token });

		ctx->err = true;
		return NULL;
	}

	next_token(ctx);
	return expr;
}

/*
Parse a single token expression (constant or variable) from `ctx`.
*/
static AstNodeExpr *parse_single_token_expr(ParserCtx *ctx) {
	AstNodeExpr *expr = Malloc(sizeof(AstNodeExpr));
	*expr = (AstNodeExpr){
		.lhs = { .tok = ctx->token },
		.oper = ctx->token->type == TOKEN_IDENTIFIER ?
			EXPR_IDENTIFIER :
			EXPR_CONST
	};

	next_token(ctx);
	return expr;
}

/*
Try and generate AST node for a function call.

Returns true if a node was added, false otherwise.
*/
static AstNodeExpr *parse_func_call(ParserCtx *ctx) {
	const Token *func_name_token = ctx->token;

	AstNode *child = make_ast_node();
	AstNode *child_copy = child;

	ctx->token = ctx->token->next->next;

	unsigned short num_values = 0;

	while (true) {
		if (!ctx->token) {
			FMT_ERROR(ERR_UNCLOSED_FUNC_CALL, {
				.loc = &func_name_token->location
			});

			free_ast_tree(child_copy);
			ctx->err = true;
			return NULL;
		}
		if (ctx->token->type == TOKEN_COMMA) {
			FMT_ERROR(ERR_UNEXPECTED_COMMA, { .loc = &ctx->token->location });

			free_ast_tree(child_copy);
			ctx->err = true;
			return NULL;
		}

		AstNode *old_node = ctx->node;

		ctx->node = child;

		Token *last = ctx->token;
		AstNodeExpr *expr = parse_expression(ctx);

		if (expr) {
			num_values++;
			AstNode *expr_node = push_ast_node(ctx, last, AST_NODE_EXPR);
			expr_node->expr = expr;
		}

		child = ctx->node;

		ctx->node = old_node;

		if (!ctx->token) {
			FMT_ERROR(ERR_MISSING_CLOSING_PAREN, { .loc = &last->location });

			free_ast_tree(child_copy);
			ctx->err = true;
			return NULL;
		}

		if (ctx->token->type == TOKEN_PAREN_CLOSE) break;

		if (ctx->token->type != TOKEN_COMMA) {
			FMT_ERROR(ERR_EXPECTED_COMMA, {
				.loc = &ctx->token->location
			});

			free_ast_tree(child_copy);
			ctx->err = true;
			return NULL;
		}

		next_token(ctx);
	}

	AstNodeExpr *expr = Calloc(1, sizeof(AstNodeExpr));
	expr->oper = EXPR_FUNC;

	expr->lhs.func_call = Malloc(sizeof(AstNodeFunctionCall));
	*expr->lhs.func_call = (AstNodeFunctionCall){
		.func_name_tok = func_name_token,
		.params = child_copy,
		.num_values = num_values
	};

	next_token(ctx);
	return expr;
}

/*
Push a new AST node to `node` with type `node_type`. Return created node.
*/
static AstNode *push_ast_node(
	ParserCtx *ctx,
	Token *last,
	NodeType node_type
) {
	AstNode *new_node = make_ast_node();
	new_node->last = ctx->node;

	ctx->node->type = node_type;
	ctx->node->token = last;
	ctx->node->token_end = ctx->token;
	ctx->node->next = new_node;

	ctx->node = new_node;

	return ctx->node->last;
}

/*
Free an expression nodes and all its sub-expressions.
*/
static void free_expr_node(AstNodeExpr *expr) {
	if (!expr) return;

	if (expr->oper == EXPR_CONST) {
		if (expr->lhs.tok->type == TOKEN_STR_CONST)
			free(expr->value.str);

		free(expr);
		return;
	}
	if (expr->oper == EXPR_IDENTIFIER) {
		free(expr);
		return;
	}
	if (expr->oper == EXPR_FUNC) {
		free_ast_tree(expr->lhs.func_call->params);
		free(expr->lhs.func_call);
		free(expr);
		return;
	}

	free_expr_node(expr->lhs.expr);
	free_expr_node(expr->rhs);
	free(expr);
}

/*
Frees an AST tree.
*/
void free_ast_tree(AstNode *node) {
	AstNode *current = NULL;

	while (node) {
		free_ast_node(node);

		if (node->child) free_ast_tree(node->child);

		current = node;
		node = node->next;
		current->next = NULL;
		free(current);
	}
}

static void free_ast_node(AstNode *node) {
	switch (node->type) {
		case AST_NODE_FUNCTION_PROTO:
			free_ast_function_proto(node);
			break;
		case AST_NODE_VAR_DEF:
			free_ast_var_def(node);
			break;
		case AST_NODE_EXPR:
		case AST_NODE_RETURN:
		case AST_NODE_IF:
		case AST_NODE_ELIF:
		case AST_NODE_WHILE:
			free_expr_node(node->expr);
			break;
		case AST_NODE_VAR_ASSIGN :
			free_ast_var_assign(node);
			break;
		default: break;
	}
}

static void free_ast_function_proto(AstNode *node) {
	unsigned num_params = node->func_proto->num_params;

	AstNodeFunctionParam **params = node->func_proto->params;

	for RANGE(i, num_params) { // NOLINT
		free_param(params[i]);
	}

	free(node->func_proto);
}

static void free_ast_var_def(AstNode *node) {
	if (node->var_def && node->var_def->expr) {
		free_expr_node(node->var_def->expr);
		free(node->var_def);
	}
}

static void free_ast_var_assign(AstNode *node) {
	if (node->var_assign && node->var_assign->expr) {
		free_expr_node(node->var_assign->expr);
		free(node->var_assign);
	}
}

/*
Return whether `token_type` represents a constant literal, or an identifier.
*/
static __attribute__((pure)) bool is_single_token_expr(TokenType token_type) {
	switch (token_type) {
		case TOKEN_IDENTIFIER:
		case TOKEN_INT_CONST:
		case TOKEN_FLOAT_CONST:
		case TOKEN_BOOL_CONST:
		case TOKEN_RUNE_CONST:
		case TOKEN_STR_CONST:
			return true;
		default: return false;
	}
}

/*
Print AST tree to screen (for debugging).
*/
void print_ast_tree(const AstNode *node) {
	print_ast_tree_(node, 0);
}

/*
The actual `print_ast_tree` function.

Print `node` and all of its properties (including tokens).
Indent more depending on the value of `indent_lvl`.
*/
static void print_ast_tree_(const AstNode *node, unsigned indent_lvl) {
	while (node) {
		for RANGE(_, indent_lvl) { // NOLINT
			putchar(' ');
		}

		printf("<node at %p, node_type: %u>\n",
			(const void *)node,
			node->type
		);

		if (!node->token) break;

		const Token *token = node->token;
		const Token *token_end = node->token_end->next;

		while (token != token_end) {
			char *str = token_to_mbs_str(token);

			for RANGE(_, indent_lvl) { // NOLINT
				putchar(' ');
			}

			printf(
				"   <token at %p, token_type: %u, "
				"column: %u, line: %u, data: `%s`>\n",
				(const void *)token,
				token->type,
				token->location.column,
				token->location.line,
				str
			);

			free(str);

			token = token->next;
		}

		if (node->child)
			print_ast_tree_(node->child, indent_lvl + 2);

		node = node->next;
	}
}

const Location *find_expr_node_location(const AstNodeExpr *expr) {
	return &find_expr_node_token(expr)->location;
}

const Token *find_expr_node_token(const AstNodeExpr *expr) {
	switch (expr->oper) {
		case EXPR_IDENTIFIER:
		case EXPR_CONST:
			return expr->lhs.tok;
		case EXPR_FUNC:
			return expr->lhs.func_call->func_name_tok;
		case EXPR_UNARY_NEG:
		case EXPR_DEREF:
		case EXPR_NOT:
		case EXPR_REF:
			return find_expr_node_token(expr->rhs);
		default: return find_expr_node_token(expr->lhs.expr);
	}
}

static void next_token(ParserCtx *ctx) {
	ctx->token = ctx->token->next;
}
