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

AstNode *try_parse_expression(Token **, AstNode **);
AstNode *parse_func_call(Token **, AstNode **);

AstNode *make_ast_tree_(Token **, unsigned);

void free_ast_tree_(AstNode *);

_Bool is_value(TokenType);

_Bool ast_token_cmp(Token *, ...);

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
	classify_tokens(token);

	Token *token_copy = token;

	AstNode *const ret = make_ast_tree_(&token, 0);

	if (!ret) free_tokens(token_copy);

	return ret;
}

bool try_parse_return(Token **token, AstNode **node) {
	if ((*token)->type != TOKEN_KW_RETURN) return false;

	push_ast_node(*token, *token, AST_NODE_RETURN, node);
	*token = (*token)->next;

	if (!try_parse_expression(token, node)) {
		PANIC(ERR_RETURN_MISSING_EXPR, { .tok = *token });
	}

	return true;
}

bool try_parse_type_alias(Token **token, AstNode **node) {
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

bool try_parse_var_def(Token **_token, AstNode **node) {
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

bool try_parse_var_assign(Token **token, AstNode **node) {
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

ExprType token_type_to_expr_oper_type(TokenType type) {
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

AstNode *push_expr_ast_node(
	Token *lhs,
	ExprType oper,
	Token *rhs,
	Token **token,
	AstNode **node
) {
	Token *last = *token;
	*token = rhs;

	(*node)->attr.expr = Malloc(sizeof(AstNodeExpr));
	*(*node)->attr.expr = (AstNodeExpr){
		.lhs = { .tok = lhs },
		.oper = oper,
		.rhs = { .tok = rhs }
	};

	AstNode *pushed = push_ast_node(*token, last, AST_NODE_EXPR, node);
	*token = (*token)->next;

	return pushed;
}

AstNode *push_expr_ast_node2(
	AstNodeExpr *lhs,
	ExprType oper,
	Token *rhs,
	Token **token,
	AstNode **node
) {
	*token = rhs;

	AstNodeExpr *new_expr = Malloc(sizeof(AstNodeExpr));

	*new_expr = (AstNodeExpr){
		.lhs = { .expr = lhs },
		.oper = oper,
		.rhs = { .tok = rhs }
	};

	(*node)->last->attr.expr = new_expr;

	*token = (*token)->next;
	return (*node)->last;
}

AstNode *try_parse_binary_oper(
	AstNodeExpr *expr,
	Token **token,
	AstNode **node
) {
	if (!(*token &&
		(*token)->next &&
		is_value((*token)->next->type)
	)) {
		return NULL;
	}

	const ExprType oper = token_type_to_expr_oper_type((*token)->type);
	if (oper == EXPR_UNKNOWN) return NULL;

	return push_expr_ast_node2(expr, oper, (*token)->next, token, node);
}

AstNode *try_parse_unary_oper(Token **token, AstNode **node) {
	if (!(*token)->next || !is_value((*token)->next->type)) return NULL;

	ExprType oper = token_type_to_expr_oper_type((*token)->type);

	if (oper == EXPR_SUB) oper = EXPR_UNARY_NEG;
	else if (oper != EXPR_NOT) return NULL;

	return push_expr_ast_node(NULL, oper, (*token)->next, token, node);
}

bool try_parse_function_proto(Token **_token, AstNode **node) {
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

bool try_parse_condition(Token **token, AstNode **node) {
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

	if (token_type != TOKEN_KW_ELSE && !try_parse_expression(token, node)) {
		PANIC(ERR_RETURN_MISSING_EXPR, { .tok = *token });
	}

	return true;
}

/*
Internal AST tree generator.
*/
AstNode *make_ast_tree_(Token **token, unsigned indent_lvl) {
	AstNode *node = make_ast_node();
	AstNode *head = node;

	while (*token) {
		const TokenType token_type = (*token)->type;

		if (token_type == TOKEN_BRACKET_OPEN) {
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

		if (token_type == TOKEN_NEWLINE ||
			token_type == TOKEN_COMMA ||
			(node->last && node->last->token_end == *token)
		) {
			*token = (*token)->next;
			continue;
		}

		if (try_parse_type_alias(token, &node) ||
			try_parse_var_def(token, &node) ||
			try_parse_var_assign(token, &node) ||
			try_parse_return(token, &node) ||
			try_parse_function_proto(token, &node) ||
			try_parse_expression(token, &node) ||
			try_parse_condition(token, &node)
		) {
			continue;
		}
		if (token_type == TOKEN_KW_UNREACHABLE) {
			push_ast_node(*token, *token, AST_NODE_UNREACHABLE, &node);
			continue;
		}
		if (token_type == TOKEN_COMMENT) {
			push_ast_node(*token, *token, AST_NODE_COMMENT, &node);
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

AstNode *parse_single_token_expr(Token **, AstNode **);
AstNode *parse_paren_expr(Token **, AstNode **);

/*
Try and generate AST node for expression.

Returns node if one was added, NULL otherwise.
*/
AstNode *try_parse_expression(Token **token, AstNode **node) {
	AstNode *x = NULL;

	if ((*token)->type == TOKEN_PAREN_OPEN) {
		x = parse_paren_expr(token, node);
	}
	else if (AST_TOKEN_CMP(*token, TOKEN_IDENTIFIER, TOKEN_PAREN_OPEN)) {
		x = parse_func_call(token, node);
	}
	else if ((x = try_parse_unary_oper(token, node))) {
		// pass
	}
	else if (is_value((*token)->type)) {
		x = parse_single_token_expr(token, node);
	}

	if (x) {
		AstNode *backup = x;

		if ((x = try_parse_binary_oper(x->attr.expr, token, node))) {
			return x;
		}

		return backup;
	}

	return x;
}

AstNode *parse_paren_expr(Token **token, AstNode **node) {
	*token = (*token)->next;

	AstNode *pushed = try_parse_expression(token, node);

	if (!pushed) {
		PANIC(ERR_INVALID_EXPR, { .tok = *token });
	}

	if ((*token)->type != TOKEN_PAREN_CLOSE) {
		PANIC(ERR_MISSING_CLOSING_PAREN, { .tok = *token });
	}

	*token = (*token)->next;

	return pushed;
}

AstNode *parse_single_token_expr(Token **token, AstNode **node) {
	ExprType oper = EXPR_CONST;

	if ((*token)->type == TOKEN_IDENTIFIER)
		oper = EXPR_IDENTIFIER;

	AstNode *pushed = push_ast_node(*token, *token, AST_NODE_EXPR, node);

	pushed->attr.expr = Malloc(sizeof(AstNodeExpr));
	*pushed->attr.expr = (AstNodeExpr){
		.lhs = { .tok = *token },
		.oper = oper
	};

	*token = (*token)->next;

	return pushed;
}

/*
Try and generate AST node for a function call.

Returns true if a node was added, false otherwise.
*/
AstNode *parse_func_call(Token **token, AstNode **node) {
	const Token *func_name_token = *token;

	AstNode *pushed = push_ast_node(*token, *token, AST_NODE_EXPR, node);

	AstNode *child = make_ast_node();
	pushed->child = child;

	*token = (*token)->next->next;

	unsigned short num_values = 0;

	while (true) {
		if (try_parse_expression(token, &child))
			num_values++;

		if ((*token)->type == TOKEN_PAREN_CLOSE) break;

		if ((*token)->type != TOKEN_COMMA) {
			PANIC(ERR_EXPECTED_COMMA, {
				.tok = *token
			});
		}

		*token = (*token)->next;
	}

	*token = (*token)->next;

	pushed->attr.expr = Malloc(sizeof(AstNodeExpr));
	*pushed->attr.expr = (AstNodeExpr){
		.oper = EXPR_FUNC
	};

	pushed->attr.expr->func_call = Malloc(sizeof(AstNodeFunctionCall));
	*pushed->attr.expr->func_call = (AstNodeFunctionCall){
		.func_name_tok = func_name_token,
		.params = pushed->child,
		.num_values = num_values
	};

	return pushed;
}

/*
Push a new AST node to `node` with type `node_type`
*/
AstNode *push_ast_node(
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

	return (*node)->last;
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
void free_ast_tree_(AstNode *node) {
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
		if (token == NULL || token->type != current_type) {
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
__attribute__((pure)) bool is_value(TokenType token_type) {
	return token_type == TOKEN_IDENTIFIER ||
		token_type == TOKEN_INT_CONST ||
		token_type == TOKEN_FLOAT_CONST ||
		token_type == TOKEN_BOOL_CONST ||
		token_type == TOKEN_RUNE_CONST ||
		token_type == TOKEN_STR_CONST;
}

void print_ast_tree_(const AstNode *, unsigned);

/*
Print AST tree to screen (for debugging).
*/
void print_ast_tree(const AstNode *node) {
	print_ast_tree_(node, 0);
}

void print_ast_tree_(const AstNode *node, unsigned indent_lvl) {
	while (node) {
		for RANGE(_, indent_lvl) { // NOLINT
			putchar(' ');
		}

		printf("<node at %p, node_type: %u>\n",
			(void *)node,
			node->type
		);

		if (!node->token) {
			break;
		}

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
				token->column,
				token->line,
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
