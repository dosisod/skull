#include <stdbool.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/range.h"
#include "skull/common/str.h"
#include "skull/semantic/variable.h"

#include "skull/parse/ast_node.h"

typedef struct {
	AstNode *node;
	AstNode *head;
	Token *token;
	unsigned indent_lvl;
	bool err;
} ParserCtx;

static AstNode *parse_expression(ParserCtx *);
static AstNodeExpr *_parse_expression(ParserCtx *);
static AstNodeExpr *parse_func_call(Token **, bool *);
static AstNode *parse_ast_tree_(ParserCtx *);
static void free_ast_function_proto(AstNode *);
static void free_ast_var_def(AstNode *);
static void free_ast_var_assign(AstNode *);
static void free_ast_node(AstNode *);
static void free_ast_tree_(AstNode *);
static bool is_single_token_expr(TokenType);
static void free_expr_node(AstNodeExpr *);
static void splice_expr_node(AstNode *);
static void push_ast_node(Token *const, Token *, NodeType, AstNode **);
static void print_ast_tree_(const AstNode *, unsigned);
static AstNodeExpr *parse_single_token_expr(Token **);
static AstNodeExpr *parse_paren_expr(Token **, bool *);
static void parse_ast_sub_tree_(ParserCtx *);
static bool parse_ast_node(ParserCtx *);


typedef enum {
	RESULT_OK,
	RESULT_IGNORE,
	RESULT_ERROR
} ParserResult;

#define AST_TOKEN_CMP2(tok, type1, type2) \
	((tok) && (tok)->type == (type1) && \
	(tok)->next && (tok)->next->type == (type2))

/*
Makes an AST (abstract syntax tree) from a given string.
*/
AstNode *parse_ast_tree(Token *token) {
	Token *head = token;

	ParserCtx ctx = {
		.token = token
	};

	AstNode *const tree = parse_ast_tree_(&ctx);

	if (!tree) free_tokens(head);
	else if (!tree->token) {
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
	push_ast_node(ctx->token, ctx->token, AST_NODE_RETURN, &ctx->node);
	ctx->token = ctx->token->next; // NOLINT

	const bool added_expr = parse_expression(ctx);

	if (ctx->err) return;

	if (added_expr) splice_expr_node(ctx->node);
}

#define IS_TYPE_LIKE(token) \
	((token)->type == TOKEN_TYPE || (token)->type == TOKEN_IDENTIFIER)

static ParserResult parse_var_def(ParserCtx *ctx) {
	bool is_const = true;
	bool is_implicit = true;
	bool is_exported = false;
	Token *first = ctx->token;

	if (ctx->token->type == TOKEN_KW_MUT) {
		is_const = false;
		ctx->token = ctx->token->next;
	}
	else if (ctx->token->type == TOKEN_KW_EXPORT) {
		is_exported = true;
		ctx->token = ctx->token->next;
	}

	if (!ctx->token) {
		ctx->token = first;
		return RESULT_IGNORE;
	}

	Token *name_token = ctx->token;

	if (ctx->token->type == TOKEN_NEW_IDENTIFIER &&
		ctx->token->next &&
		IS_TYPE_LIKE(ctx->token->next) &&
		ctx->token->next->next &&
		ctx->token->next->next->type == TOKEN_OPER_EQUAL
	) {
		is_implicit = false;
		ctx->token = ctx->token->next->next;
	}
	else if (AST_TOKEN_CMP2(ctx->token,
		TOKEN_IDENTIFIER,
		TOKEN_OPER_AUTO_EQUAL)
	) {
		ctx->token = ctx->token->next;
	}
	else return RESULT_IGNORE;

	ctx->node->var_def = Malloc(sizeof(AstNodeVarDef));
	*ctx->node->var_def = (AstNodeVarDef){
		.is_const = is_const,
		.is_implicit = is_implicit,
		.is_exported = is_exported,
		.name_tok = name_token
	};

	push_ast_node(ctx->token, first, AST_NODE_VAR_DEF, &ctx->node);
	ctx->token = ctx->token->next;

	if (ctx->token->type == TOKEN_TYPE) {
		ctx->token = ctx->token->next;
		return RESULT_OK;
	}

	const bool success = parse_expression(ctx);
	if (ctx->err) return RESULT_ERROR;

	if (!success) {
		FMT_ERROR(ERR_ASSIGN_MISSING_EXPR, { .loc = &ctx->token->location });
		return RESULT_ERROR;
	}

	splice_expr_node(ctx->node);

	if (ctx->token && ctx->token->type != TOKEN_NEWLINE) {
		FMT_ERROR(ERR_EXPECTED_NEWLINE, {
			.loc = &ctx->token->location
		});
		return RESULT_ERROR;
	}

	return RESULT_OK;
}

static ParserResult parse_var_assign(ParserCtx *ctx) {
	if (!AST_TOKEN_CMP2(ctx->token,
		TOKEN_IDENTIFIER,
		TOKEN_OPER_EQUAL)
	) {
		return RESULT_IGNORE;
	}

	AstNodeVarAssign *var_assign;
	var_assign = Calloc(1, sizeof *var_assign);

	push_ast_node(
		ctx->token->next,
		ctx->token,
		AST_NODE_VAR_ASSIGN,
		&ctx->node
	);
	ctx->token = ctx->token->next->next;

	bool err = false;
	const bool success = parse_expression(ctx);
	if (err) {
		free(var_assign);
		return RESULT_ERROR;
	}

	if (!success) {
		FMT_ERROR(ERR_ASSIGN_MISSING_EXPR, { .loc = &ctx->token->location });

		free(var_assign);
		return RESULT_ERROR;
	}

	ctx->node->last->last->var_assign = var_assign;
	splice_expr_node(ctx->node);

	return RESULT_OK;
}

/*
Convert a `TokenType` to an `ExprType`.
*/
static ExprType token_type_to_expr_oper_type(TokenType type) {
	switch (type) {
		case TOKEN_OPER_PLUS: return EXPR_ADD;
		case TOKEN_OPER_MINUS: return EXPR_SUB;
		case TOKEN_OPER_MULT: return EXPR_MULT;
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
		default: return EXPR_UNKNOWN;
	}
}

/*
Parse the right-hand-side of an expression given `lhs`.

Set `err` if an error occurred.
*/
static AstNodeExpr *build_rhs_expr(
	AstNodeExpr *lhs,
	ExprType oper,
	Token **token,
	bool *err
) {
	const Token *oper_tok = *token;
	*token = (*token)->next;

	ParserCtx ctx = {
		.token = *token,
	};

	AstNodeExpr *rhs = _parse_expression(&ctx);
	*err = ctx.err;
	*token = ctx.token;

	if (*err) return false;
	if (!rhs) {
		FMT_ERROR(ERR_EXPECTED_EXPR, { .tok = oper_tok });
		*err = true;
		return false;
	}

	AstNodeExpr *new_expr = Malloc(sizeof(AstNodeExpr));
	*new_expr = (AstNodeExpr){
		.lhs = { .expr = lhs },
		.oper = oper,
		.rhs = rhs
	};

	return new_expr;
}

/*
Try to parse a binary operator from `expr`.

Set `err` if an error occurred.
*/
static AstNodeExpr *parse_binary_oper(
	AstNodeExpr *expr,
	Token **token,
	bool *err
) {
	if (!*token || !(*token)->next) return NULL;

	const ExprType oper = token_type_to_expr_oper_type((*token)->type);
	if (oper == EXPR_UNKNOWN) return NULL;

	return build_rhs_expr(expr, oper, token, err);
}

/*
Try to parse a unary operator from `expr`.

Set `err` if an error occurred.
*/
static AstNodeExpr *parse_unary_oper(Token **token, bool *err) {
	if (!(*token)->next) return NULL;

	ExprType oper = token_type_to_expr_oper_type((*token)->type);

	if (oper == EXPR_SUB) oper = EXPR_UNARY_NEG;
	else if (oper != EXPR_NOT) return NULL;

	return build_rhs_expr(NULL, oper, token, err);
}

static void free_param(AstNodeFunctionParam *param) {
	if (!param) return;

	if (param->var && !param->var->name) free(param->var);
	free(param->type_name);
	free(param->param_name);
	free(param);
}

static ParserResult parse_function_proto(ParserCtx *ctx) {
	Token *first = ctx->token;

	const bool is_external = ctx->token->type == TOKEN_KW_EXTERNAL;
	const bool is_export = ctx->token->type == TOKEN_KW_EXPORT;

	if (is_external || is_export) ctx->token = ctx->token->next;

	if (!AST_TOKEN_CMP2(ctx->token,
		TOKEN_IDENTIFIER,
		TOKEN_PAREN_OPEN)
	) {
		ctx->token = first;
		return RESULT_IGNORE;
	}

	const Token *const func_name_token = ctx->token;
	ctx->token = ctx->token->next->next;

	Vector *params = make_vector();

	bool is_proto = false;

	while (ctx->token->type == TOKEN_NEW_IDENTIFIER &&
		ctx->token->next &&
		IS_TYPE_LIKE(ctx->token->next)
	) {
		is_proto = true;

		AstNodeFunctionParam *param;
		param = Malloc(sizeof *param);
		param->type_name = token_to_mbs_str(ctx->token->next);
		param->param_name = token_to_string(ctx->token);

		// allocate a placeholder variable to store the location
		param->var = Calloc(1, sizeof(Variable));
		param->var->location = ctx->token->location;

		vector_push(params, param);

		ctx->token = ctx->token->next->next;
		if (ctx->token->type != TOKEN_COMMA) break;
		ctx->token = ctx->token->next;
	}

	char *return_type_name = NULL;

	const TokenType token_type = is_external ?
		TOKEN_NEWLINE :
		TOKEN_BRACKET_OPEN;

	if (ctx->token->type == TOKEN_PAREN_CLOSE &&
		ctx->token->next &&
		IS_TYPE_LIKE(ctx->token->next) &&
		ctx->token->next->next &&
		ctx->token->next->next->type == token_type
	) {
		return_type_name = token_to_mbs_str(ctx->token->next);
		ctx->token = ctx->token->next;
	}

	else if (!AST_TOKEN_CMP2(ctx->token,
		TOKEN_PAREN_CLOSE,
		token_type)
	) {
		free_vector(params, (void(*)(void *))free_param);

		if (is_proto) {
			FMT_ERROR(ERR_EXPECTED_COMMA, {
				.loc = &ctx->token->location
			});

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
		.return_type_name = return_type_name,
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

	push_ast_node(ctx->token, first, AST_NODE_FUNCTION_PROTO, &ctx->node);
	ctx->token = ctx->token->next;

	return RESULT_OK;
}

static bool parse_condition(ParserCtx *ctx, NodeType node_type) {
	if (!ctx->token || !ctx->token->next) return true;

	push_ast_node(ctx->token, ctx->token, node_type, &ctx->node);
	ctx->token = ctx->token->next;

	bool err = false;
	const bool success = parse_expression(ctx);

	if (success && !err) {
		splice_expr_node(ctx->node);

		return false;
	}

	FMT_ERROR(ERR_INVALID_EXPR, { .tok = ctx->token });
	return true;
}

static bool parse_if(ParserCtx *ctx) {
	return parse_condition(ctx, AST_NODE_IF);
}

static bool parse_elif(ParserCtx *ctx) {
	return parse_condition(ctx, AST_NODE_ELIF);
}

static void parse_else(ParserCtx *ctx) {
	push_ast_node(ctx->token, ctx->token, AST_NODE_ELSE, &ctx->node);
	ctx->token = ctx->token->next; // NOLINT
}

static bool parse_while(ParserCtx *ctx) {
	return parse_condition(ctx, AST_NODE_WHILE);
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
		free_ast_tree_(ctx->head);
		return NULL;
	}

	if (!ctx->token && ctx->indent_lvl != 0) {
		FMT_ERROR(ERR_EOF_NO_BRACKET, {0});

		free_ast_tree_(ctx->head);
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
	push_ast_node(
		ctx->token,
		ctx->token,
		AST_NODE_UNREACHABLE,
		&ctx->node
	);
	ctx->token = ctx->token->next;
}

static void parse_comment(ParserCtx *ctx) {
	push_ast_node(ctx->token, ctx->token, AST_NODE_COMMENT, &ctx->node);
	ctx->token = ctx->token->next;
}

static void parse_noop(ParserCtx *ctx) {
	push_ast_node(ctx->token, ctx->token, AST_NODE_NOOP, &ctx->node);
	ctx->token = ctx->token->next;
}

/*
Parse a single AST node.

Set `err` if an error occurred.

Return `true` if a terminating token was reached (closing bracket).
*/
static bool parse_ast_node(ParserCtx *ctx) {
	const TokenType token_type = (ctx->token)->type;

	if (token_type == TOKEN_BRACKET_OPEN) {
		parse_ast_sub_tree_(ctx);
	}
	else if (token_type == TOKEN_BRACKET_CLOSE) {
		if (ctx->indent_lvl == 0) {
			FMT_ERROR(ERR_MISSING_OPEN_BRAK, {
				.loc = &(ctx->token)->location
			});

			ctx->err = true;
		}
		return true;
	}
	else if (token_type == TOKEN_NEWLINE || token_type == TOKEN_COMMA) {
		ctx->token = ctx->token->next;
	}
	else if (token_type == TOKEN_KW_RETURN) {
		parse_return(ctx);
	}
	else if (token_type == TOKEN_KW_IF) {
		ctx->err |= parse_if(ctx);
	}
	else if (token_type == TOKEN_KW_ELIF) {
		ctx->err |= parse_elif(ctx);
	}
	else if (token_type == TOKEN_KW_ELSE) {
		parse_else(ctx);
	}
	else if (token_type == TOKEN_KW_WHILE) {
		ctx->err |= parse_while(ctx);
	}
	else if (token_type == TOKEN_KW_UNREACHABLE) {
		parse_unreachable(ctx);
	}
	else if (token_type == TOKEN_COMMENT) {
		parse_comment(ctx);
	}
	else if (token_type == TOKEN_KW_NOOP) {
		parse_noop(ctx);
	}
	else {
		ParserResult result = parse_function_proto(ctx);

		if (result == RESULT_IGNORE)
			result = parse_var_def(ctx);

		if (result == RESULT_IGNORE)
			result = parse_var_assign(ctx);

		if (result == RESULT_IGNORE) {
			if (!parse_expression(ctx)) {
				result = RESULT_ERROR;

				if (!ctx->err) FMT_ERROR(ERR_UNEXPECTED_TOKEN, {
					.tok = ctx->token
				});
			}
		}

		if (result == RESULT_ERROR) ctx->err = true;
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

	ctx->token = ctx->token->next;

	ParserCtx new_ctx = {
		.token = ctx->token,
		.indent_lvl = ctx->indent_lvl + 1
	};

	AstNode *const child = parse_ast_tree_(&new_ctx);
	ctx->token = new_ctx.token;

	if (!child || new_ctx.err) {
		ctx->err = true;
		return;
	}

	if (!child->token) {
		FMT_ERROR(ERR_EMPTY_BLOCK, { .loc = &(ctx->token)->location });

		free_ast_tree_(child);
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

	ctx->token = ctx->token->next; // NOLINT
}

/*
Try and generate AST node for expression.

Returns node if one was added, NULL otherwise.
*/
static AstNode *parse_expression(ParserCtx *ctx) {
	Token **token = &ctx->token;
	AstNode **node = &ctx->node;

	Token *last = *token;

	AstNodeExpr *expr_node = _parse_expression(ctx);
	if (!expr_node || ctx->err) return NULL;

	push_ast_node(NULL, last, AST_NODE_EXPR, node);

	(*node)->last->expr = expr_node;

	ctx->node = *node;
	ctx->token = *token;

	return (*node)->last;
}

/*
Internal `parse_expression` function. Used for recursive expr parsing.
*/
static AstNodeExpr *_parse_expression(ParserCtx *ctx) {
	if (!ctx->token) return NULL;

	AstNodeExpr *expr = NULL;

	if (ctx->token->type == TOKEN_PAREN_OPEN) {
		expr = parse_paren_expr(&ctx->token, &ctx->err);
	}
	else if (AST_TOKEN_CMP2(ctx->token, TOKEN_IDENTIFIER, TOKEN_PAREN_OPEN)) {
		expr = parse_func_call(&ctx->token, &ctx->err);
	}
	else if ((expr = parse_unary_oper(&ctx->token, &ctx->err))) {
		// pass
	}
	else if (is_single_token_expr(ctx->token->type)) {
		expr = parse_single_token_expr(&ctx->token);
	}

	if (!expr) return NULL;

	AstNodeExpr *binary_oper = parse_binary_oper(expr, &ctx->token, &ctx->err);
	if (ctx->err) {
		free_expr_node(expr);
		return NULL;
	}

	if (binary_oper) return binary_oper;
	return expr;
}

/*
Parse parenthesis expression from `token`.

Set `err` if an error occurred.
*/
static AstNodeExpr *parse_paren_expr(Token **token, bool *err) {
	*token = (*token)->next;

	ParserCtx ctx = {
		.token = *token
	};

	AstNodeExpr *pushed = _parse_expression(&ctx);
	*err = ctx.err;
	*token = ctx.token;

	if (!pushed || *err) {
		FMT_ERROR(ERR_INVALID_EXPR, { .tok = *token });

		if (pushed) free_expr_node(pushed);

		*err = true;
		return NULL;
	}

	if (!*token || (*token)->type != TOKEN_PAREN_CLOSE) {
		FMT_ERROR(ERR_MISSING_CLOSING_PAREN, { .loc = &(*token)->location });

		free_expr_node(pushed);
		*err = true;
		return NULL;
	}

	*token = (*token)->next;
	return pushed;
}

/*
Parse a single token expression (constant or variable) from `token`.
*/
static AstNodeExpr *parse_single_token_expr(Token **token) {
	AstNodeExpr *expr = Malloc(sizeof(AstNodeExpr));
	*expr = (AstNodeExpr){
		.lhs = { .tok = *token },
		.oper = (*token)->type == TOKEN_IDENTIFIER ?
			EXPR_IDENTIFIER :
			EXPR_CONST
	};

	*token = (*token)->next;
	return expr;
}

/*
Try and generate AST node for a function call.

Returns true if a node was added, false otherwise.
*/
static AstNodeExpr *parse_func_call(Token **token, bool *err) {
	const Token *func_name_token = *token;

	AstNode *child = make_ast_node();
	AstNode *child_copy = child;

	*token = (*token)->next->next;

	unsigned short num_values = 0;

	while (true) {
		if (!*token) {
			FMT_ERROR(ERR_UNCLOSED_FUNC_CALL, {
				.loc = &func_name_token->location
			});

			free_ast_tree_(child_copy);
			*err = true;
			return NULL;
		}

		ParserCtx ctx = {
			.token = *token,
			.node = child
		};

		if (parse_expression(&ctx)) num_values++;

		*err = ctx.err;
		*token = ctx.token;
		child = ctx.node;

		if ((*token)->type == TOKEN_PAREN_CLOSE) break;

		if ((*token)->type != TOKEN_COMMA) {
			FMT_ERROR(ERR_EXPECTED_COMMA, {
				.loc = &(*token)->location
			});

			free_ast_tree_(child_copy);
			*err = true;
			return NULL;
		}

		*token = (*token)->next;
	}

	AstNodeExpr *expr_node = Malloc(sizeof(AstNodeExpr));
	*expr_node = (AstNodeExpr){
		.oper = EXPR_FUNC
	};

	expr_node->lhs.func_call = Malloc(sizeof(AstNodeFunctionCall));
	*expr_node->lhs.func_call = (AstNodeFunctionCall){
		.func_name_tok = func_name_token,
		.params = child_copy,
		.num_values = num_values
	};

	*token = (*token)->next;

	return expr_node;
}

/*
Push a new AST node to `node` with type `node_type`
*/
static void push_ast_node(
	Token *const token,
	Token *last,
	NodeType node_type,
	AstNode **node
) {
	AstNode *const new_node = make_ast_node();
	new_node->last = *node;

	(*node)->type = node_type;
	(*node)->token = last;
	(*node)->token_end = token;
	(*node)->next = new_node;

	(*node) = new_node;
}

/*
Frees an AST tree.
*/
void free_ast_tree(AstNode *node) {
	if (node) {
		Token *token = node->token;

		free_ast_tree_(node);
		free_tokens(token);
	};
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
		free_ast_tree_((void *)expr->lhs.func_call->params);
		free(expr->lhs.func_call);
		free(expr);
		return;
	}

	free_expr_node((void *)expr->lhs.expr);
	free_expr_node((void *)expr->rhs);
	free(expr);
}

/*
Internal AST freeing function.
*/
static void free_ast_tree_(AstNode *node) {
	AstNode *current = NULL;

	while (node) {
		free_ast_node(node);

		if (node->child) free_ast_tree_(node->child);

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
			free_expr_node(node->expr);
			break;
		case AST_NODE_VAR_ASSIGN :
			free_ast_var_assign(node);
			break;
		case AST_NODE_IF:
		case AST_NODE_ELIF:
		case AST_NODE_RETURN:
		case AST_NODE_WHILE: {
			if (!node->expr_node) return;

			free_expr_node(node->expr_node->expr);
			free(node->expr_node);
			break;
		}
		default: break;
	}
}

static void free_ast_function_proto(AstNode *node) {
	free(node->func_proto->return_type_name);

	unsigned num_params = node->func_proto->num_params;

	AstNodeFunctionParam **params = node->func_proto->params;

	for RANGE(i, num_params) { // NOLINT
		free_param(params[i]);
	}

	free(node->func_proto);
}

static void free_ast_var_def(AstNode *node) {
	if (node->var_def) {
		if (node->var_def->expr_node)
			free_expr_node(node->var_def->expr_node->expr);

		free(node->var_def->expr_node);
	}

	free(node->var_def);
}

static void free_ast_var_assign(AstNode *node) {
	if (node->var_assign && node->var_assign->expr_node) {
		free_expr_node(node->var_assign->expr_node->expr);
		free(node->var_assign->expr_node);
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
Given `node`, take the last node (expr) and attach it to the node before
that one.
*/
static void splice_expr_node(AstNode *node) {
	AstNode *cond_node = node->last->last;
	AstNode *expr_node = node->last;

	if (cond_node->type == AST_NODE_VAR_DEF) {
		cond_node->var_def->expr_node = expr_node;
	}
	else if (cond_node->type == AST_NODE_VAR_ASSIGN) {
		cond_node->var_assign->expr_node = expr_node;
	}
	else {
		cond_node->expr_node = expr_node;
	}
	cond_node->next = node;
	node->last = cond_node;
	expr_node->next = NULL;
	expr_node->last = NULL;
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
			(void *)node,
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
				(void *)token,
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
	switch (expr->oper) {
		case EXPR_IDENTIFIER:
		case EXPR_CONST:
			return &expr->lhs.tok->location;
		case EXPR_FUNC:
			return &expr->lhs.func_call->func_name_tok->location;
		case EXPR_UNARY_NEG:
		case EXPR_NOT:
			return find_expr_node_location(expr->rhs);
		default: return find_expr_node_location(expr->lhs.expr);
	}
}
