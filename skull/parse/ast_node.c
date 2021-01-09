#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/eval/types/types.h"
#include "skull/parse/classify.h"

#include "skull/parse/ast_node.h"

#define MAX_PARAMS 256

bool try_gen_expression(Token **, Token **, AstNode **);

/*
Makes an AST (abstract syntax tree) from a given string.
*/
AstNode *make_ast_tree(const char32_t *const code) {
	Token *const token = tokenize(code);
	classify_tokens(token);

	Token *token_last = NULL;
	AstNode *const ret = make_ast_tree_(token, 0, &token_last, TOKEN_BRACKET_CLOSE);

	if (!ret) {
		free_tokens(token);
	}

	return ret;
}

bool is_ast_type_alias(Token **token, Token **last, AstNode **node) {
	if (AST_TOKEN_CMP(*token,
		TOKEN_IDENTIFIER,
		TOKEN_OPER_AUTO_EQUAL,
		TOKEN_TYPE)
	) {
		*token = (*token)->next->next->next;
		push_ast_node(*token, last, AST_NODE_TYPE_ALIAS, node);
		return true;
	}

	return false;
}

#define IS_TYPE_LIKE(token) ((token)->token_type == TOKEN_TYPE || (token)->token_type == TOKEN_IDENTIFIER)

bool is_ast_var_def(Token **_token, Token **last, AstNode **node) {
	bool is_const = true;
	bool is_implicit = true;

	// too lazy to dereference each `token`
	Token *token = *_token;

	if (token->token_type == TOKEN_KW_MUT) {
		is_const = false;
		token = token->next;

		if (!token) {
			return false;
		}
	}

	if (token->token_type == TOKEN_NEW_IDENTIFIER &&
		token->next &&
		IS_TYPE_LIKE(token->next) &&
		token->next->next &&
		token->next->next->token_type == TOKEN_OPER_EQUAL
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

	MAKE_ATTR(AstNodeVarDef, *node,
		.is_const = is_const,
		.is_implicit = is_implicit
	);

	push_ast_node(*_token, last, AST_NODE_VAR_DEF, node);
	return true;
}

bool is_const_oper(Token **_token, Token **last, AstNode **node) {
	Token *token = *_token;

	if (!(
		is_value(token) &&
		token->next &&
		token->next->next &&
		is_value(token->next->next)
	)) {
		return false;
	}

	NodeType node_type = AST_NODE_UNKNOWN;

switch (token->next->token_type) {
	case TOKEN_OPER_PLUS: node_type = AST_NODE_ADD; break;
	case TOKEN_OPER_MINUS: node_type = AST_NODE_SUB; break;
	case TOKEN_OPER_MULT: node_type = AST_NODE_MULT; break;
	case TOKEN_OPER_DIV: node_type = AST_NODE_DIV; break;
	default: return false;
}

	MAKE_ATTR(AstNodeOper, *node,
		.lhs = token,
		.rhs = token->next->next
	);

	*_token = token->next->next;
	push_ast_node(*_token, last, node_type, node);
	return true;
}

bool is_ast_function_proto(Token **_token, Token **last, AstNode **node) {
	Token *token = *_token;

	bool is_external = false;
	bool is_export = false;

	if (token->token_type == TOKEN_KW_EXTERNAL) {
		is_external = true;
		token = token->next;
	}
	else if (token->token_type == TOKEN_KW_EXPORT) {
		is_export = true;
		token = token->next;
	}

	if (!AST_TOKEN_CMP(token,
		TOKEN_IDENTIFIER,
		TOKEN_PAREN_OPEN)
	) {
		return false;
	}

	token = token->next->next;

	// prevent realloc by pre-allocating MAX_PARAMS number of params.
	// if you need more then that, you are insane.
	char *param_type_names[MAX_PARAMS] = {0};
	char32_t *param_names[MAX_PARAMS] = {0};
	unsigned short num_params = 0;

	const TokenType token_type = is_external ? TOKEN_NEWLINE : TOKEN_BRACKET_OPEN;

	while (token->token_type == TOKEN_NEW_IDENTIFIER &&
		token->next &&
		IS_TYPE_LIKE(token->next)
	) {
		param_names[num_params] = token_str(token);
		param_type_names[num_params] = token_mbs_str(token->next);

		token = token->next->next;
		num_params++;

		if (token->token_type != TOKEN_COMMA) {
			break;
		}

		token = token->next;
	}

	char *return_type_name = NULL;

	if (token->token_type == TOKEN_PAREN_CLOSE &&
		token->next &&
		IS_TYPE_LIKE(token->next) &&
		token->next->next &&
		token->next->next->token_type == token_type
	) {
		return_type_name = token_mbs_str(token->next);
		token = token->next;
	}

	else if (!AST_TOKEN_CMP(token,
		TOKEN_PAREN_CLOSE,
		token_type)
	) {
		return false;
	}

	*_token = token;

	char32_t **tmp_names = NULL;
	char **tmp_types = NULL;

	if (num_params) {
		tmp_names = Calloc(num_params, sizeof(char32_t *));
		memcpy(tmp_names, param_names, num_params * sizeof(char32_t *));

		tmp_types = Calloc(num_params, sizeof(char *));
		memcpy(tmp_types, param_type_names, num_params * sizeof(char *));
	}

	MAKE_ATTR(AstNodeFunctionProto, *node,
		.param_type_names = tmp_types,
		.param_names = tmp_names,
		.return_type_name = return_type_name,
		.is_external = is_external,
		.is_export = is_export,
		.num_params = num_params
	);

	push_ast_node(*_token, last, AST_NODE_FUNCTION_PROTO, node);
	return true;
}

#define IS_BOOL_LIKE(tok) ((tok)->token_type == TOKEN_IDENTIFIER || (tok)->token_type == TOKEN_BOOL_CONST)

bool is_conditional(TokenType token_type, Token **_token, Token **last, AstNode **node, NodeType node_type) {
	Token *token = *_token;

	if (token->token_type != token_type || !token->next) {
		return false;
	}
	token = token->next;

	Token *lhs = NULL;
	TokenType oper = TOKEN_UNKNOWN;
	Token *rhs = token;

	if (token->token_type == TOKEN_OPER_NOT && IS_BOOL_LIKE(token->next)) {
		oper = TOKEN_OPER_NOT;
		rhs = token->next;
		token = token->next;

		if (!token) {
			return false;
		}
	}
	else if (
		is_value(token) &&
		token->next &&
		token->next->token_type == TOKEN_OPER_IS &&
		token->next->next &&
		is_value(token->next->next)
	) {
		lhs = token;
		oper = TOKEN_OPER_IS;
		rhs = token->next->next;
		token = token->next->next;

		if (!token) {
			return false;
		}
	}
	else if (!IS_BOOL_LIKE(token)) {
		return false;
	}

	MAKE_ATTR(AstNodeBoolExpr, *node,
		.lhs = lhs,
		.oper = oper,
		.rhs = rhs
	);

	*_token = token;
	push_ast_node(*_token, last, node_type, node);

	return true;
}

/*
Internal AST tree generator.
*/
AstNode *make_ast_tree_(Token *token, unsigned indent_lvl, Token **token_last, TokenType bracket_or_paren) {
	Token *last = token;

	AstNode *node = make_ast_node();
	AstNode *head = node;
	bool allow_top_lvl_bracket = false;

	while (token) {
		if (token->token_type == TOKEN_BRACKET_OPEN || token->token_type == TOKEN_PAREN_OPEN) {
			AstNode *const child = make_ast_tree_(
				token->next,
				indent_lvl + 1,
				token_last,
				(token->token_type == TOKEN_BRACKET_OPEN) ?
					TOKEN_BRACKET_CLOSE :
					TOKEN_PAREN_CLOSE
			);

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

			if (!child->token_end) {
				if (!token->next) {
					return head;
				}
			}
			else {
				token = child->token_end->next;
			}

			token = (*token_last)->next;
			continue;
		}

		if (token->token_type == bracket_or_paren) {
			if (indent_lvl == 0 && !allow_top_lvl_bracket) {
				free(head);
				PANIC(ERR_MISSING_OPEN_BRAK, { .tok = token });
			}

			*token_last = token;
			break;
		}

		if (token->token_type == TOKEN_NEWLINE ||
			token->token_type == TOKEN_COMMA ||
			(node->last && node->last->token_end == token)
		) {
			token = token->next;
			continue;
		}

		last = token;

		if (is_ast_type_alias(&token, &last, &node)) {
			continue;
		}

		if (is_ast_var_def(&token, &last, &node)) {
			continue;
		}

		if (AST_TOKEN_CMP(token,
			TOKEN_IDENTIFIER,
			TOKEN_OPER_EQUAL)
		) {
			token = token->next;
			push_ast_node(token, &last, AST_NODE_VAR_ASSIGN, &node);
			continue;
		}

		if (token->token_type == TOKEN_KW_RETURN) {
			push_ast_node(token, &last, AST_NODE_RETURN, &node);
			continue;
		}
		if (is_conditional(TOKEN_KW_IF, &token, &last, &node, AST_NODE_IF)) {
			continue;
		}
		if (is_conditional(TOKEN_KW_ELIF, &token, &last, &node, AST_NODE_ELIF)) {
			continue;
		}
		if (token->token_type == TOKEN_KW_ELSE) {
			push_ast_node(token, &last, AST_NODE_ELSE, &node);
			continue;
		}
		if (is_conditional(TOKEN_KW_WHILE, &token, &last, &node, AST_NODE_WHILE)) {
			continue;
		}
		if (is_ast_function_proto(&token, &last, &node)) {
			continue;
		}
		if (token->token_type == TOKEN_COMMENT) {
			push_ast_node(token, &last, AST_NODE_COMMENT, &node);
			continue;
		}
		if (try_gen_expression(&token, &last, &node)) {
			continue;
		}

		free(head);
		PANIC(ERR_UNEXPECTED_TOKEN, { .tok = token });
	}

	if (!token && indent_lvl != 0) {
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

/*
Try and generate AST node for expression.

Returns true if a node was added, false otherwise.
*/
bool try_gen_expression(Token **_token, Token **last, AstNode **node) {
	Token *token = *_token;

	if (is_const_oper(_token, last, node)) {
		// pass
	}

	else if (AST_TOKEN_CMP(token, TOKEN_IDENTIFIER, TOKEN_PAREN_OPEN)) {
		*_token = token->next;

		push_ast_node(token, last, AST_NODE_FUNCTION, node);
	}
	else if (token->token_type == TOKEN_IDENTIFIER) {
		push_ast_node(token, last, AST_NODE_IDENTIFIER, node);
	}

	else if (is_value(token)) {
		push_ast_node(token, last, AST_NODE_CONST, node);
	}

	else {
		return false;
	}

	return true;
}

/*
Push a new AST node to `node` with type `node_type`
*/
void push_ast_node(Token *const token, Token **last, NodeType node_type, AstNode **node) {
	(*node)->node_type = node_type;
	(*node)->token = *last;
	(*node)->token_end = token;

	AstNode *const new_node = make_ast_node();

	new_node->last = *node;
	*last = token->next;

	(*node)->next = new_node;
	(*node) = new_node;
}

/*
Makes an AstNode with default values
*/
AstNode *make_ast_node(void) {
	AstNode *node;
	node = Calloc(1, sizeof *node);

	return node;
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
		if (node->attr) {
			if (node->node_type == AST_NODE_FUNCTION_PROTO) {
				free(ATTR(AstNodeFunctionProto, node, return_type_name));

				char **param_type_names = ATTR(AstNodeFunctionProto, node, param_type_names);
				char32_t **param_names = ATTR(AstNodeFunctionProto, node, param_names);

				unsigned num_params = ATTR(AstNodeFunctionProto, node, num_params);

				for (unsigned i = 0 ; i < num_params ; i++) {
					free(param_type_names[i]);
					free(param_names[i]);
				}

				free(param_type_names);
				free(param_names);
			}

			free(node->attr);
		}

		if (node->child) {
			free_ast_tree_(node->child);
		}

		current = node;
		node = node->next;
		current->next = NULL;
		free(current);
	}
}

/*
Check each token's type starting at `token`, checking against the corresponding token type specified in `...`
*/
bool ast_token_cmp(Token *token, ...) {
	va_list vargs;
	va_start(vargs, token);

	TokenType current_type = va_arg(vargs, TokenType); // NOLINT

	while (current_type != TOKEN_END) {
		if (token == NULL || token->token_type != current_type) {
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
Return whether `token` represents a constant literal, or an identifier.
*/
__attribute__((pure)) bool is_value(const Token *const token) {
	return token->token_type == TOKEN_IDENTIFIER ||
		token->token_type == TOKEN_INT_CONST ||
		token->token_type == TOKEN_FLOAT_CONST ||
		token->token_type == TOKEN_BOOL_CONST ||
		token->token_type == TOKEN_RUNE_CONST ||
		token->token_type == TOKEN_STR_CONST;
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
		char *indent = Malloc(indent_lvl + 1);
		memset(indent, ' ', indent_lvl);
		indent[indent_lvl] = '\0';

		printf("%s<node at %p, node_type: %u>\n",
			indent,
			(void *)node,
			node->node_type
		);

		const Token *token = node->token;
		const Token *token_end = node->token_end->next;

		while (token != token_end) {
			char *str = token_mbs_str(token);

			printf("%s   <token at %p, token_type: %u, column: %u, line: %u, data: `%s`>\n",
				indent,
				(void *)token,
				token->token_type,
				token->line,
				token->column,
				str
			);

			free(str);

			token = token->next;
		}

		free(indent);

		if (node->child) {
			print_ast_tree_(node->child, indent_lvl + 2);
		}

		node = node->next;
	}
}