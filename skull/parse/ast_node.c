#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/panic.h"
#include "skull/common/range.h"
#include "skull/common/str.h"
#include "skull/compiler/types/types.h"
#include "skull/parse/classify.h"

#include "skull/parse/ast_node.h"

static AstNode *try_parse_expression(Token **, AstNode **);
static AstNodeExpr *_try_parse_expression(Token **);
static AstNodeExpr *parse_func_call(Token **);

static AstNode *make_ast_tree_(Token **, unsigned);

static void free_ast_tree_(AstNode *);

static bool is_value(TokenType);

bool ast_token_cmp(Token *, ...);

/*
Compare a variable number of token types stored in `...` agains each
successive token in `token`.
*/
#define AST_TOKEN_CMP(tok, ...) ast_token_cmp((tok), __VA_ARGS__, TOKEN_END)

/*
Makes an AST (abstract syntax tree) from a given string.
*/
AstNode *make_ast_tree(const char32_t *const code) {
	Token *token = tokenize(code);
	Token *head = token;

	classify_tokens(token);

	AstNode *const tree = make_ast_tree_(&token, 0);

	if (!tree) free_tokens(head);

	return tree;
}

static bool try_parse_return(Token **token, AstNode **node) {
	if ((*token)->type != TOKEN_KW_RETURN) return false;

	push_ast_node(*token, *token, AST_NODE_RETURN, node);
	*token = (*token)->next;

	if (!try_parse_expression(token, node)) {
		PANIC(ERR_RETURN_MISSING_EXPR, { .tok = *token });
	}

	return true;
}

static bool try_parse_type_alias(Token **token, AstNode **node) {
	if (!AST_TOKEN_CMP(*token,
		TOKEN_IDENTIFIER,
		TOKEN_OPER_AUTO_EQUAL,
		TOKEN_TYPE)
	) {
		return false;
	}

	Token *next = (*token)->next->next->next;
	push_ast_node(next, *token, AST_NODE_TYPE_ALIAS, node);

	*token = next;
	return true;
}

#define IS_TYPE_LIKE(token) \
	((token)->type == TOKEN_TYPE || (token)->type == TOKEN_IDENTIFIER)

static bool try_parse_var_def(Token **_token, AstNode **node) {
	bool is_const = true;
	bool is_implicit = true;

	// too lazy to dereference each `token`
	Token *token = *_token;
	Token *last = token;

	if (token->type == TOKEN_KW_MUT) {
		is_const = false;
		token = token->next;

		if (!token) return false;
	}

	if (token->type == TOKEN_NEW_IDENTIFIER &&
		token->next &&
		IS_TYPE_LIKE(token->next) &&
		token->next->next &&
		token->next->next->type == TOKEN_OPER_EQUAL
	) {
		is_implicit = false;
		*_token = token->next->next;
	}
	else if (AST_TOKEN_CMP(token,
		TOKEN_IDENTIFIER,
		TOKEN_OPER_AUTO_EQUAL)
	) {
		*_token = token->next;
	}
	else {
		return false;
	}

	(*node)->attr.var_def = Malloc(sizeof(AstNodeVarDef));
	*(*node)->attr.var_def = (AstNodeVarDef){
		.is_const = is_const,
		.is_implicit = is_implicit,
		.name_tok = token
	};

	push_ast_node(*_token, last, AST_NODE_VAR_DEF, node);
	*_token = (*_token)->next;

	if (!try_parse_expression(_token, node)) {
		PANIC(ERR_ASSIGN_MISSING_EXPR, { .tok = *_token });
	}

	return true;
}

static bool try_parse_var_assign(Token **token, AstNode **node) {
	if (!AST_TOKEN_CMP(*token,
		TOKEN_IDENTIFIER,
		TOKEN_OPER_EQUAL)
	) {
		return false;
	}

	push_ast_node((*token)->next, *token, AST_NODE_VAR_ASSIGN, node);
	*token = (*token)->next->next;

	if (!try_parse_expression(token, node)) {
		PANIC(ERR_ASSIGN_MISSING_EXPR, { .tok = *token });
	}

	return true;
}

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

static AstNodeExpr *build_rhs_expr(
	AstNodeExpr *lhs,
	ExprType oper,
	Token **token
) {
	const Token *oper_tok = *token;
	*token = (*token)->next;

	AstNodeExpr *rhs = _try_parse_expression(token);
	if (!rhs) {
		PANIC(ERR_EXPECTED_EXPR, { .tok = oper_tok });
	}

	AstNodeExpr *new_expr = Malloc(sizeof(AstNodeExpr));
	*new_expr = (AstNodeExpr){
		.lhs = { .expr = lhs },
		.oper = oper,
		.rhs = { .expr = rhs }
	};

	return new_expr;
}

static AstNodeExpr *try_parse_binary_oper(AstNodeExpr *expr, Token **token) {
	if (!*token || !(*token)->next) return NULL;

	const ExprType oper = token_type_to_expr_oper_type((*token)->type);
	if (oper == EXPR_UNKNOWN) return NULL;

	return build_rhs_expr(expr, oper, token);
}

static AstNodeExpr *try_parse_unary_oper(Token **token) {
	if (!(*token)->next) return NULL;

	ExprType oper = token_type_to_expr_oper_type((*token)->type);

	if (oper == EXPR_SUB) oper = EXPR_UNARY_NEG;
	else if (oper != EXPR_NOT) return NULL;

	return build_rhs_expr(NULL, oper, token);
}

static bool try_parse_function_proto(Token **_token, AstNode **node) {
	Token *token = *_token;
	Token *last = token;

	const bool is_external = token->type == TOKEN_KW_EXTERNAL;
	const bool is_export = token->type == TOKEN_KW_EXPORT;

	if (is_external || is_export) token = token->next;

	const Token *const func_name_token = token;

	if (!AST_TOKEN_CMP(token,
		TOKEN_IDENTIFIER,
		TOKEN_PAREN_OPEN)
	) {
		return false;
	}

	token = token->next->next;

	Vector *param_names = make_vector();
	Vector *param_type_names = make_vector();

	const TokenType token_type = is_external ?
		TOKEN_NEWLINE :
		TOKEN_BRACKET_OPEN;

	while (token->type == TOKEN_NEW_IDENTIFIER &&
		token->next &&
		IS_TYPE_LIKE(token->next)
	) {
		vector_push(param_names, token_str(token));
		vector_push(param_type_names, token_mbs_str(token->next));

		token = token->next->next;

		if (token->type != TOKEN_COMMA) break;

		token = token->next;
	}

	char *return_type_name = NULL;

	if (token->type == TOKEN_PAREN_CLOSE &&
		token->next &&
		IS_TYPE_LIKE(token->next) &&
		token->next->next &&
		token->next->next->type == token_type
	) {
		return_type_name = token_mbs_str(token->next);
		token = token->next;
	}

	else if (!AST_TOKEN_CMP(token,
		TOKEN_PAREN_CLOSE,
		token_type)
	) {
		free_vector(param_names, NULL);
		free_vector(param_type_names, NULL);

		return false;
	}

	*_token = token;

	char32_t **tmp_names = NULL;
	char **tmp_types = NULL;
	const unsigned short num_params = (unsigned short)param_names->length;

	if (param_names->length) {
		tmp_names = vector_freeze(param_names);
		tmp_types = vector_freeze(param_type_names);
	}

	free_vector(param_names, NULL);
	free_vector(param_type_names, NULL);

	(*node)->attr.func_proto = Malloc(sizeof(AstNodeFunctionProto));
	*(*node)->attr.func_proto = (AstNodeFunctionProto){
		.name_tok = func_name_token,
		.param_type_names = tmp_types,
		.param_names = tmp_names,
		.return_type_name = return_type_name,
		.is_external = is_external,
		.is_export = is_export,
		.num_params = num_params
	};

	push_ast_node(*_token, last, AST_NODE_FUNCTION_PROTO, node);
	return true;
}

static bool try_parse_condition(Token **token, AstNode **node) {
	const TokenType token_type = (*token)->type;
	NodeType node_type;

	if (token_type == TOKEN_KW_IF) node_type = AST_NODE_IF;
	else if (token_type == TOKEN_KW_ELIF) node_type = AST_NODE_ELIF;
	else if (token_type == TOKEN_KW_ELSE) node_type = AST_NODE_ELSE;
	else if (token_type == TOKEN_KW_WHILE) node_type = AST_NODE_WHILE;
	else return false;

	if (!(*token)->next) return false;

	push_ast_node(*token, *token, node_type, node);
	*token = (*token)->next;

	if (token_type == TOKEN_KW_ELSE) return true;

	if (!try_parse_expression(token, node)) {
		PANIC(ERR_INVALID_EXPR, { .tok = *token });
	}

	return true;
}

/*
Internal AST tree generator.
*/
static AstNode *make_ast_tree_(Token **token, unsigned indent_lvl) {
	AstNode *node = make_ast_node();
	AstNode *head = node;

	while (*token) {
		const TokenType token_type = (*token)->type;

		if (token_type == TOKEN_BRACKET_OPEN) {
			if (node->last && node->last->child) {
				PANIC(ERR_UNEXPECTED_CODE_BLOCK, { .tok = *token });
			}

			*token = (*token)->next;
			AstNode *const child = make_ast_tree_(token, indent_lvl + 1);

			if (!child) {
				free(head);
				return NULL;
			}

			if (!node->last) {
				head->child = child;
				child->parent = head;
			}
			else {
				node->last->child = child;
				child->parent = node->last;
			}

			if (!child->token_end && !(*token)->next) return head;

			*token = (*token)->next;
			continue;
		}

		if (token_type == TOKEN_BRACKET_CLOSE) {
			if (indent_lvl == 0) {
				free(head);
				PANIC(ERR_MISSING_OPEN_BRAK, { .tok = *token });
			}

			break;
		}

		if (token_type == TOKEN_NEWLINE || token_type == TOKEN_COMMA) {
			*token = (*token)->next;
			continue;
		}

		if (
			try_parse_function_proto(token, &node) ||
			try_parse_type_alias(token, &node)
		) {
			*token = (*token)->next;
			continue;
		}
		if (
			try_parse_var_def(token, &node) ||
			try_parse_var_assign(token, &node) ||
			try_parse_return(token, &node) ||
			try_parse_expression(token, &node) ||
			try_parse_condition(token, &node)
		) {
			continue;
		}
		if (token_type == TOKEN_KW_UNREACHABLE) {
			push_ast_node(*token, *token, AST_NODE_UNREACHABLE, &node);
			*token = (*token)->next;
			continue;
		}
		if (token_type == TOKEN_COMMENT) {
			push_ast_node(*token, *token, AST_NODE_COMMENT, &node);
			*token = (*token)->next;
			continue;
		}

		free(head);
		PANIC(ERR_UNEXPECTED_TOKEN, { .tok = *token });
	}

	if (!*token && indent_lvl != 0) {
		free(head);
		PANIC(ERR_EOF_NO_BRACKET, {0});
	}

	if (node->last && head != node) {
		node->last->next = NULL;
		node->last = NULL;
		free(node);
	}

	return head;
}

static AstNodeExpr *parse_single_token_expr(Token **);
static AstNodeExpr *parse_paren_expr(Token **);

/*
Try and generate AST node for expression.

Returns node if one was added, NULL otherwise.
*/
static AstNode *try_parse_expression(Token **token, AstNode **node) {
	Token *last = *token;

	AstNodeExpr *expr_node = _try_parse_expression(token);
	if (!expr_node) return NULL;

	push_ast_node(NULL, last, AST_NODE_EXPR, node);

	(*node)->last->token_end = *token;
	(*node)->last->attr.expr = expr_node;

	return (*node)->last;
}

/*
Internal `try_parse_expression` function. Used for recursive expr parsing.
*/
static AstNodeExpr *_try_parse_expression(Token **token) {
	AstNodeExpr *expr = NULL;

	if ((*token)->type == TOKEN_PAREN_OPEN) {
		expr = parse_paren_expr(token);
	}
	else if (AST_TOKEN_CMP(*token, TOKEN_IDENTIFIER, TOKEN_PAREN_OPEN)) {
		expr = parse_func_call(token);
	}
	else if ((expr = try_parse_unary_oper(token))) {
		// pass
	}
	else if (is_value((*token)->type)) {
		expr = parse_single_token_expr(token);
	}

	if (!expr) return NULL;

	AstNodeExpr *binary_oper = try_parse_binary_oper(expr, token);
	if (binary_oper) return binary_oper;

	return expr;
}

static AstNodeExpr *parse_paren_expr(Token **token) {
	*token = (*token)->next;

	AstNodeExpr *pushed = _try_parse_expression(token);

	if (!pushed) {
		PANIC(ERR_INVALID_EXPR, { .tok = *token });
	}

	if ((*token)->type != TOKEN_PAREN_CLOSE) {
		PANIC(ERR_MISSING_CLOSING_PAREN, { .tok = *token });
	}

	*token = (*token)->next;

	return pushed;
}

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
static AstNodeExpr *parse_func_call(Token **token) {
	const Token *func_name_token = *token;

	AstNode *child = make_ast_node();
	AstNode *child_copy = child;

	*token = (*token)->next->next;

	unsigned short num_values = 0;

	while (true) {
		if (try_parse_expression(token, &child)) num_values++;

		if ((*token)->type == TOKEN_PAREN_CLOSE) break;

		if ((*token)->type != TOKEN_COMMA) {
			PANIC(ERR_EXPECTED_COMMA, {
				.tok = *token
			});
		}

		*token = (*token)->next;
	}

	AstNodeExpr *expr_node = Malloc(sizeof(AstNodeExpr));
	*expr_node = (AstNodeExpr){
		.oper = EXPR_FUNC
	};

	expr_node->func_call = Malloc(sizeof(AstNodeFunctionCall));
	*expr_node->func_call = (AstNodeFunctionCall){
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
Internal AST freeing function, dont call directly.
*/
static void free_ast_tree_(AstNode *node) {
	AstNode *current = NULL;

	while (node) {
		if (node->type == AST_NODE_FUNCTION_PROTO) {
			free(node->attr.func_proto->return_type_name);

			char **param_type_names = \
				node->attr.func_proto->param_type_names;

			char32_t **param_names = node->attr.func_proto->param_names;

			unsigned num_params = node->attr.func_proto->num_params;

			for RANGE(i, num_params) { // NOLINT
				free(param_type_names[i]);
				free(param_names[i]);
			}

			free(param_type_names);
			free(param_names);

			free(node->attr.func_proto);
		}
		else if (node->type == AST_NODE_VAR_DEF) {
			free(node->attr.var_def);
		}
		else if (node->type == AST_NODE_EXPR) {
			if (node->attr.expr->oper == EXPR_FUNC) {
				free_ast_tree_((AstNode *)node->attr.expr->func_call->params);
				free(node->attr.expr->func_call);
			}

			free(node->attr.expr);
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
Check each token's type starting at `token`, checking against the
corresponding token type specified in `...`
*/
bool ast_token_cmp(Token *token, ...) {
	va_list vargs;
	va_start(vargs, token);

	TokenType current_type = va_arg(vargs, TokenType); // NOLINT

	while (current_type != TOKEN_END) {
		if (!token || token->type != current_type) {
			va_end(vargs);
			return false;
		}

		current_type = va_arg(vargs, TokenType); // NOLINT
		token = token->next;
	}

	va_end(vargs);
	return true;
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

static void print_ast_tree_(const AstNode *, unsigned);

/*
Print AST tree to screen (for debugging).
*/
void print_ast_tree(const AstNode *node) {
	print_ast_tree_(node, 0);
}

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
