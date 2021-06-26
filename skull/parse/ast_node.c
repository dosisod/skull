#include <stdbool.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/range.h"
#include "skull/common/str.h"
#include "skull/compiler/types.h"
#include "skull/compiler/variable.h"
#include "skull/parse/classify.h"

#include "skull/parse/ast_node.h"

static AstNode *try_parse_expression(Token **, AstNode **, bool *);
static AstNodeExpr *_try_parse_expression(Token **, bool *);
static AstNodeExpr *parse_func_call(Token **, bool *);
static AstNode *parse_ast_tree_(Token **, unsigned, bool *);
static void free_ast_tree_(AstNode *);
static bool is_value(TokenType);
static void free_expr_node(AstNodeExpr *);
static void unsplice_expr_node(AstNode *);

#define AST_TOKEN_CMP2(tok, type1, type2) \
	((tok) && (tok)->type == (type1) && \
	(tok)->next && (tok)->next->type == (type2))

#define AST_TOKEN_CMP3(tok, type1, type2, type3) \
	(AST_TOKEN_CMP2(tok, type1, type2) && \
	(tok)->next->next && (tok)->next->next->type == (type3))

/*
Makes an AST (abstract syntax tree) from a given string.
*/
AstNode *parse_ast_tree(const char32_t *const code) {
	Token *token = tokenize(code);
	if (!token) return NULL;

	classify_tokens(token);

	bool err = false;
	Token *head = token;
	AstNode *const tree = parse_ast_tree_(&token, 0, &err);

	if (!tree) free_tokens(head);
	else if (!tree->token) {
		// in case the first node does not have a token, assign the head of
		// the token list to it
		tree->token = head;
	}
	if (err) {
		free_ast_tree(tree);
		return NULL;
	}

	return tree;
}

/*
Try and generate a valid return node from `token`.

Return `true` if error occurred.
*/
static bool parse_return(Token **token, AstNode **node) {
	push_ast_node(*token, *token, AST_NODE_RETURN, node);
	*token = (*token)->next; // NOLINT

	bool err = false;
	const bool added_expr = try_parse_expression(token, node, &err);
	if (err) return true;

	if (added_expr) unsplice_expr_node(*node);

	return false;
}

/*
Try and generate a type alias node from `token`.
*/
static bool try_parse_type_alias(Token **token, AstNode **node) {
	if (!AST_TOKEN_CMP3(*token,
		TOKEN_IDENTIFIER,
		TOKEN_OPER_AUTO_EQUAL,
		TOKEN_TYPE)
	) {
		return false;
	}

	Token *next = (*token)->next->next->next;
	push_ast_node(next, *token, AST_NODE_TYPE_ALIAS, node);

	*token = next->next;
	return true;
}

#define IS_TYPE_LIKE(token) \
	((token)->type == TOKEN_TYPE || (token)->type == TOKEN_IDENTIFIER)

/*
Try and generate a variable definition node from `token`.

Set `err` if an error occurred.
*/
static bool try_parse_var_def(Token **_token, AstNode **node, bool *err) {
	bool is_const = true;
	bool is_implicit = true;
	bool is_exported = false;

	// too lazy to dereference each `token`
	Token *token = *_token;
	Token *last = token;

	if (token->type == TOKEN_KW_MUT) {
		is_const = false;
		token = token->next;
	}
	else if (token->type == TOKEN_KW_EXPORT) {
		is_exported = true;
		token = token->next;
	}

	if (!token) return false;

	if (token->type == TOKEN_NEW_IDENTIFIER &&
		token->next &&
		IS_TYPE_LIKE(token->next) &&
		token->next->next &&
		token->next->next->type == TOKEN_OPER_EQUAL
	) {
		is_implicit = false;
		*_token = token->next->next;
	}
	else if (AST_TOKEN_CMP2(token,
		TOKEN_IDENTIFIER,
		TOKEN_OPER_AUTO_EQUAL)
	) {
		*_token = token->next;
	}
	else return false;

	(*node)->var_def = Malloc(sizeof(AstNodeVarDef));
	*(*node)->var_def = (AstNodeVarDef){
		.is_const = is_const,
		.is_implicit = is_implicit,
		.is_exported = is_exported,
		.name_tok = token
	};

	push_ast_node(*_token, last, AST_NODE_VAR_DEF, node);
	*_token = (*_token)->next;

	const bool success = try_parse_expression(_token, node, err);
	if (*err) return false;

	if (!success) {
		FMT_ERROR(ERR_ASSIGN_MISSING_EXPR, { .loc = &(*_token)->location });
		*err = true;
		return false;
	}

	AstNode *cond_node = (*node)->last->last;
	AstNode *expr_node = (*node)->last;

	cond_node->var_def->expr_node = expr_node;
	cond_node->next = *node;
	(*node)->last = cond_node;
	expr_node->next = NULL;
	expr_node->last = NULL;

	if (*_token && (*_token)->type != TOKEN_NEWLINE) {
		FMT_ERROR(ERR_EXPECTED_NEWLINE, {
			.loc = &(*_token)->location
		});
		*err = true;
	}

	return true;
}

/*
Try and generate a variable assignment node from `token`.

Set `err` if an error occurred.
*/
static bool try_parse_var_assign(Token **token, AstNode **node, bool *err) {
	if (!AST_TOKEN_CMP2(*token,
		TOKEN_IDENTIFIER,
		TOKEN_OPER_EQUAL)
	) {
		return false;
	}

	push_ast_node((*token)->next, *token, AST_NODE_VAR_ASSIGN, node);
	*token = (*token)->next->next;

	const bool success = try_parse_expression(token, node, err);
	if (*err) return false;

	if (!success) {
		FMT_ERROR(ERR_ASSIGN_MISSING_EXPR, { .loc = &(*token)->location });
		*err = true;
		return false;
	}

	unsplice_expr_node(*node);

	return true;
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

	AstNodeExpr *rhs = _try_parse_expression(token, err);
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
		.rhs = { .expr = rhs }
	};

	return new_expr;
}

/*
Try to parse a binary operator from `expr`.

Set `err` if an error occurred.
*/
static AstNodeExpr *try_parse_binary_oper(
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
static AstNodeExpr *try_parse_unary_oper(Token **token, bool *err) {
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

/*
Try to parse a function prototype from `_token`.
*/
static bool try_parse_function_proto(
	Token **_token,
	AstNode **node,
	bool *err
) {
	Token *token = *_token;
	Token *last = token;

	const bool is_external = token->type == TOKEN_KW_EXTERNAL;
	const bool is_export = token->type == TOKEN_KW_EXPORT;

	if (is_external || is_export) token = token->next;

	if (!AST_TOKEN_CMP2(token,
		TOKEN_IDENTIFIER,
		TOKEN_PAREN_OPEN)
	) {
		return false;
	}

	const Token *const func_name_token = token;
	token = token->next->next;

	Vector *params = make_vector();

	bool is_proto = false;

	while (token->type == TOKEN_NEW_IDENTIFIER &&
		token->next &&
		IS_TYPE_LIKE(token->next)
	) {
		is_proto = true;

		AstNodeFunctionParam *param;
		param = Malloc(sizeof *param);
		param->type_name = token_mbs_str(token->next);
		param->param_name = token_str(token);

		// allocate a placeholder variable to store the location
		param->var = Calloc(1, sizeof(Variable));
		param->var->location = token->location;

		vector_push(params, param);

		token = token->next->next;
		if (token->type != TOKEN_COMMA) break;
		token = token->next;
	}

	char *return_type_name = NULL;

	const TokenType token_type = is_external ?
		TOKEN_NEWLINE :
		TOKEN_BRACKET_OPEN;

	if (token->type == TOKEN_PAREN_CLOSE &&
		token->next &&
		IS_TYPE_LIKE(token->next) &&
		token->next->next &&
		token->next->next->type == token_type
	) {
		return_type_name = token_mbs_str(token->next);
		token = token->next;
	}

	else if (!AST_TOKEN_CMP2(token,
		TOKEN_PAREN_CLOSE,
		token_type)
	) {
		free_vector(params, (void(*)(void *))free_param);

		if (is_proto) {
			FMT_ERROR(ERR_EXPECTED_COMMA, {
				.loc = &token->location
			});

			*err = true;
			return false;
		}

		return false;
	}

	const unsigned short num_params = (unsigned short)params->length;

	(*node)->func_proto = Malloc(
		sizeof(AstNodeFunctionProto) +
		(num_params * sizeof(AstNodeFunctionParam))
	);
	*(*node)->func_proto = (AstNodeFunctionProto){
		.name_tok = func_name_token,
		.return_type_name = return_type_name ?
			return_type_name :
			strdup(TYPE_VOID),
		.is_external = is_external,
		.is_export = is_export,
		.num_params = num_params
	};

	if (num_params) {
		memcpy(
			&(*node)->func_proto->params,
			vector_freeze(params),
			(num_params * sizeof(AstNodeFunctionParam))
		);

		// since the array is frozen, the elements will not be allowed
		// to be freed, so manually free them.
		free(params->elements);
	}

	free_vector(params, NULL);

	*_token = token;
	push_ast_node(*_token, last, AST_NODE_FUNCTION_PROTO, node);
	*_token = (*_token)->next;
	return true;
}

/*
Try to parse a conditional (if/elif/else/while statement) from `token`.

Return `true` if error occurred.
*/
static bool parse_condition(
	Token **token,
	AstNode **node,
	NodeType node_type
) {
	if (!*token || !(*token)->next) return true;

	push_ast_node(*token, *token, node_type, node);
	*token = (*token)->next;

	bool err = false;
	const bool success = try_parse_expression(token, node, &err);

	if (success && !err) {
		unsplice_expr_node(*node);

		return false;
	}

	FMT_ERROR(ERR_INVALID_EXPR, { .tok = *token });
	return true;
}

/*
Parse an "if" AST node.

Return `true` if an error occurred.
*/
static bool parse_if(Token **token, AstNode **node) {
	return parse_condition(token, node, AST_NODE_IF);
}

/*
Parse an "elif" AST node.

Return `true` if an error occurred.
*/
static bool parse_elif(Token **token, AstNode **node) {
	return parse_condition(token, node, AST_NODE_ELIF);
}

/*
Parse an "else" AST node.

Return `true` if an error occurred.
*/
static void parse_else(Token **token, AstNode **node) {
	push_ast_node(*token, *token, AST_NODE_ELSE, node);
	*token = (*token)->next; // NOLINT
}

/*
Parse a "while" AST node.

Return `true` if an error occurred.
*/
static bool parse_while(Token **token, AstNode **node) {
	return parse_condition(token, node, AST_NODE_WHILE);
}

static bool parse_ast_sub_tree_(
	Token **,
	unsigned,
	AstNode *,
	AstNode *
);

static bool parse_ast_node(
	Token **,
	unsigned,
	AstNode **,
	AstNode *,
	bool *
);

/*
Internal AST tree generator.
*/
static AstNode *parse_ast_tree_(Token **token, unsigned indent_lvl, bool *err) {
	AstNode *node = make_ast_node();
	AstNode *head = node;

	while (*token) {
		const bool done = parse_ast_node(token, indent_lvl, &node, head, err);

		if (done || *err) break;
	}

	if (*err) {
		free_ast_tree_(head);
		return NULL;
	}

	if (!*token && indent_lvl != 0) {
		FMT_ERROR(ERR_EOF_NO_BRACKET, {0});

		free_ast_tree_(head);
		*err = true;
		return NULL;
	}

	if (node->last && head != node) {
		node->last->next = NULL;
		node->last = NULL;
		free(node);
	}

	return head;
}

/*
Parse a single AST node.

Set `err` if an error occurred.

Return `true` if a terminating token was reached (closing bracket).
*/
static bool parse_ast_node(
	Token **token,
	unsigned indent_lvl,
	AstNode **node,
	AstNode *head,
	bool *err
) {
	const TokenType token_type = (*token)->type;

	if (token_type == TOKEN_BRACKET_OPEN) {
		*err |= parse_ast_sub_tree_(token, indent_lvl, *node, head);
	}
	else if (token_type == TOKEN_BRACKET_CLOSE) {
		if (indent_lvl == 0) {
			FMT_ERROR(ERR_MISSING_OPEN_BRAK, { .loc = &(*token)->location });

			*err = true;
		}
		return true;
	}
	else if (token_type == TOKEN_NEWLINE || token_type == TOKEN_COMMA) {
		*token = (*token)->next;
	}
	else if (token_type == TOKEN_KW_RETURN) {
		*err |= parse_return(token, node);
	}
	else if (token_type == TOKEN_KW_IF) {
		*err |= parse_if(token, node);
	}
	else if (token_type == TOKEN_KW_ELIF) {
		*err |= parse_elif(token, node);
	}
	else if (token_type == TOKEN_KW_ELSE) {
		parse_else(token, node);
	}
	else if (token_type == TOKEN_KW_WHILE) {
		*err |= parse_while(token, node);
	}
	else if (token_type == TOKEN_KW_UNREACHABLE) {
		push_ast_node(*token, *token, AST_NODE_UNREACHABLE, node);
		*token = (*token)->next;
	}
	else if (token_type == TOKEN_COMMENT) {
		push_ast_node(*token, *token, AST_NODE_COMMENT, node);
		*token = (*token)->next;
	}
	else if (token_type == TOKEN_KW_NOOP) {
		push_ast_node(*token, *token, AST_NODE_NOOP, node);
		*token = (*token)->next;
	}
	else if (
		try_parse_type_alias(token, node) ||
		try_parse_function_proto(token, node, err) ||
		(!*err && try_parse_var_def(token, node, err)) ||
		(!*err && try_parse_var_assign(token, node, err)) ||
		(!*err && try_parse_expression(token, node, err)) ||
		*err
	) {}
	else {
		FMT_ERROR(ERR_UNEXPECTED_TOKEN, { .tok = *token });
		*err = true;
	}

	return false;
}

/*
Start parsing a AST sub tree.

Return `true` if an error occurred.
*/
static bool parse_ast_sub_tree_(
	Token **token,
	unsigned indent_lvl,
	AstNode *node,
	AstNode *head
) {
	if (node->last && node->last->child) {
		FMT_ERROR(ERR_UNEXPECTED_CODE_BLOCK, {
			.loc = &(*token)->location
		});

		return true;
	}

	*token = (*token)->next;
	bool err = false;
	AstNode *const child = parse_ast_tree_(token, indent_lvl + 1, &err);

	if (!child || err) return true;

	if (!child->token) {
		FMT_ERROR(ERR_EMPTY_BLOCK, { .loc = &(*token)->location });

		free_ast_tree_(child);
		return true;
	}
	if (!node->last) {
		head->child = child;
		child->parent = head;
	}
	else {
		node->last->child = child;
		child->parent = node->last;
	}

	*token = (*token)->next;
	return false;
}

static AstNodeExpr *parse_single_token_expr(Token **);
static AstNodeExpr *parse_paren_expr(Token **, bool *);

/*
Try and generate AST node for expression.

Returns node if one was added, NULL otherwise.
*/
static AstNode *try_parse_expression(
	Token **token,
	AstNode **node,
	bool *err
) {
	Token *last = *token;

	AstNodeExpr *expr_node = _try_parse_expression(token, err);
	if (!expr_node || *err) return NULL;

	push_ast_node(NULL, last, AST_NODE_EXPR, node);

	(*node)->last->token_end = *token;
	(*node)->last->expr = expr_node;

	return (*node)->last;
}

/*
Internal `try_parse_expression` function. Used for recursive expr parsing.
*/
static AstNodeExpr *_try_parse_expression(Token **token, bool *err) {
	if (!*token) return NULL;

	AstNodeExpr *expr = NULL;

	if ((*token)->type == TOKEN_PAREN_OPEN) {
		expr = parse_paren_expr(token, err);
	}
	else if (AST_TOKEN_CMP2(*token, TOKEN_IDENTIFIER, TOKEN_PAREN_OPEN)) {
		expr = parse_func_call(token, err);
	}
	else if ((expr = try_parse_unary_oper(token, err))) {
		// pass
	}
	else if (is_value((*token)->type)) {
		expr = parse_single_token_expr(token);
	}

	if (!expr) return NULL;

	AstNodeExpr *binary_oper = try_parse_binary_oper(expr, token, err);
	if (*err) {
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

	AstNodeExpr *pushed = _try_parse_expression(token, err);

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

		if (try_parse_expression(token, &child, err)) num_values++;

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
void push_ast_node(
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
		free_tokens(node->token);
		free_ast_tree_(node);
	};
}

/*
Free an expression nodes and all its sub-expressions.
*/
static void free_expr_node(AstNodeExpr *expr) {
	if (!expr) return;

	if (expr->oper == EXPR_IDENTIFIER || expr->oper == EXPR_CONST) {
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
	free_expr_node((void *)expr->rhs.expr);
	free(expr);
}

/*
Internal AST freeing function.
*/
static void free_ast_tree_(AstNode *node) {
	AstNode *current = NULL;

	while (node) {
		if (node->type == AST_NODE_FUNCTION_PROTO) {
			free(node->func_proto->return_type_name);

			unsigned num_params = node->func_proto->num_params;

			AstNodeFunctionParam **params = node->func_proto->params;

			for RANGE(i, num_params) { // NOLINT
				free_param(params[i]);
			}

			free(node->func_proto);
		}
		else if (node->type == AST_NODE_VAR_DEF) {
			if (node->var_def) {
				if (node->var_def->expr_node)
					free_expr_node(node->var_def->expr_node->expr);

				free(node->var_def->expr_node);
			}

			free(node->var_def);
		}
		else if (node->type == AST_NODE_EXPR) {
			free_expr_node(node->expr);
		}
		else if (node->expr_node && (
			node->type == AST_NODE_IF ||
			node->type == AST_NODE_ELIF ||
			node->type == AST_NODE_RETURN ||
			node->type == AST_NODE_VAR_ASSIGN ||
			node->type == AST_NODE_WHILE)
		) {
			free_expr_node(node->expr_node->expr);
			free(node->expr_node);
		}

		if (node->child)
			free_ast_tree_(node->child);

		current = node;
		node = node->next;
		current->next = NULL;
		free(current);
	}
}

/*
Return whether `token_type` represents a constant literal, or an identifier.
*/
static __attribute__((pure)) bool is_value(TokenType token_type) {
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
static void unsplice_expr_node(AstNode *node) {
	AstNode *cond_node = node->last->last;
	AstNode *expr_node = node->last;

	cond_node->expr_node = expr_node;
	cond_node->next = node;
	node->last = cond_node;
	expr_node->next = NULL;
	expr_node->last = NULL;
}

static void print_ast_tree_(const AstNode *, unsigned);

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
			char *str = token_mbs_str(token);

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
