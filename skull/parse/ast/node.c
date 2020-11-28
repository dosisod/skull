#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/eval/types/types.h"
#include "skull/parse/classify.h"

#include "skull/parse/ast/node.h"

/*
Makes an AST (abstract syntax tree) from a given string.
*/
AstNode *make_ast_tree(const char32_t *const code) {
	Token *const token = tokenize(code);
	classify_tokens(token);

	Token *token_last = NULL;
	AstNode *const ret = make_ast_tree_(token, 0, &token_last);

	if (!ret) {
		free_tokens(token);
	}

	return ret;
}

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

	if (AST_TOKEN_CMP(token,
		TOKEN_NEW_IDENTIFIER,
		TOKEN_TYPE,
		TOKEN_OPER_EQUAL)
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

	if (!((
			is_const_literal(token) ||
			token->token_type == TOKEN_IDENTIFIER
		) &&
		token->next &&
		token->next->next && (
			is_const_literal(token->next->next) ||
			token->next->next->token_type == TOKEN_IDENTIFIER
		)
	)) {
		return false;
	}

	NodeType node_type = AST_NODE_UNKNOWN;

switch (token->next->token_type) {
	case TOKEN_OPER_PLUS: node_type = AST_NODE_ADD; break;
	case TOKEN_OPER_MINUS: node_type = AST_NODE_SUB; break;
	case TOKEN_OPER_MULT: node_type = AST_NODE_MULT; break;
	case TOKEN_OPER_DIV: node_type = AST_NODE_DIV; break;
	case TOKEN_OPER_IS: node_type = AST_NODE_IS; break;
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

bool is_ast_function(Token **_token, Token **last, AstNode **node) {
	Token *token = *_token;

	if (!AST_TOKEN_CMP(token,
		TOKEN_IDENTIFIER,
		TOKEN_PAREN_OPEN)
	) {
		return false;
	}

	token = token->next->next;
	Token *param = NULL;

	if (token->token_type == TOKEN_PAREN_CLOSE) {
		*_token = token;
	}
	else if ((is_const_literal(token) ||
		token->token_type == TOKEN_IDENTIFIER) &&
		token->next &&
		token->next->token_type == TOKEN_PAREN_CLOSE
	) {
		param = token;
		*_token = token->next;
	}
	else {
		return false;
	}

	MAKE_ATTR(AstNodeFunction, *node,
		.param = param
	);

	push_ast_node(*_token, last, AST_NODE_FUNCTION, node);
	return true;
}

bool is_ast_function_proto(Token **_token, Token **last, AstNode **node) {
	Token *token = *_token;

	bool is_external = false;
	if (token->token_type == TOKEN_KW_EXTERNAL) {
		is_external = true;
		token = token->next;
	}

	bool is_export = false;
	if (!is_external && token->token_type == TOKEN_KW_EXPORT) {
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

	const Type *param_types = NULL;
	char32_t *param_names = NULL;

	const Type *return_type = NULL;

	const unsigned token_type = is_external ? TOKEN_NEWLINE : TOKEN_BRACKET_OPEN;

	if (AST_TOKEN_CMP(token,
		TOKEN_NEW_IDENTIFIER,
		TOKEN_TYPE)
	) {
		param_names = token_str(token);

		char *type_name = token_mbs_str(token->next);
		param_types = find_type(type_name);
		free(type_name);

		token = token->next->next;
	}

	if (AST_TOKEN_CMP(token,
		TOKEN_PAREN_CLOSE,
		TOKEN_TYPE,
		token_type)
	) {
		char *type_name = token_mbs_str(token->next);
		return_type = find_type(type_name);
		free(type_name);
		token = token->next;
	}

	else if (!AST_TOKEN_CMP(token,
		TOKEN_PAREN_CLOSE,
		token_type)
	) {
		return false;
	}

	*_token = token;

	MAKE_ATTR(AstNodeFunctionProto, *node,
		.param_types = param_types,
		.param_names = param_names,
		.return_type = return_type,
		.is_external = is_external,
		.is_export = is_export
	);

	push_ast_node(*_token, last, AST_NODE_FUNCTION_PROTO, node);
	return true;
}

__attribute__((pure)) bool is_const_literal(Token *token) {
	return (
		token->token_type == TOKEN_INT_CONST ||
		token->token_type == TOKEN_FLOAT_CONST ||
		token->token_type == TOKEN_BOOL_CONST ||
		token->token_type == TOKEN_RUNE_CONST ||
		token->token_type == TOKEN_STR_CONST
	);
}

/*
Internal AST tree generator.
*/
AstNode *make_ast_tree_(Token *token, unsigned indent_lvl, Token **token_last) {
	Token *last = token;

	AstNode *node = make_ast_node();
	AstNode *head = node;
	bool allow_top_lvl_bracket = false;

	while (token) {
		if (token->token_type == TOKEN_BRACKET_OPEN) {
			AstNode *const child = make_ast_tree_(token->next, indent_lvl + 1, token_last);
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

		if (token->token_type == TOKEN_BRACKET_CLOSE) {
			if (indent_lvl == 0 && !allow_top_lvl_bracket) {
				free(head);
				PANIC(ERR_MISSING_OPEN_BRAK, {0});
			}

			*token_last = token;
			break;
		}

		if (token->token_type == TOKEN_NEWLINE || (
			node->last && node->last->token_end == token)
		) {
			token = token->next;
			continue;
		}

		last = token;

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

		if (token->token_type == TOKEN_KW_RETURN &&
			token->next && (
			token->next->token_type == TOKEN_IDENTIFIER ||
			token->next->token_type == TOKEN_INT_CONST)
		) {
			token = token->next;
			push_ast_node(token, &last, AST_NODE_RETURN, &node);
			continue;
		}

		if (token->token_type == TOKEN_KW_IF &&
			token->next && (
			token->next->token_type == TOKEN_IDENTIFIER ||
			token->next->token_type == TOKEN_BOOL_CONST)
		) {
			token = token->next;
			push_ast_node(token, &last, AST_NODE_IF, &node);
			continue;
		}

		if (token->token_type == TOKEN_KW_WHILE &&
			token->next && (
			token->next->token_type == TOKEN_IDENTIFIER ||
			token->next->token_type == TOKEN_BOOL_CONST)
		) {
			token = token->next;
			push_ast_node(token, &last, AST_NODE_WHILE, &node);
			continue;
		}

		if (token->token_type == TOKEN_KW_ELSE) {
			push_ast_node(token, &last, AST_NODE_ELSE, &node);
			continue;
		}

		if (is_const_oper(&token, &last, &node)) {
			continue;
		}

		if (is_ast_function_proto(&token, &last, &node)) {
			continue;
		}

		if (is_ast_function(&token, &last, &node)) {
			continue;
		}

		if (token->token_type == TOKEN_IDENTIFIER) {
			push_ast_node(token, &last, AST_NODE_IDENTIFIER, &node);
			continue;
		}
		if (token->token_type == TOKEN_COMMENT) {
			push_ast_node(token, &last, AST_NODE_COMMENT, &node);
			continue;
		}
		if (token->token_type == TOKEN_INT_CONST ||
			token->token_type == TOKEN_FLOAT_CONST ||
			token->token_type == TOKEN_BOOL_CONST ||
			token->token_type == TOKEN_RUNE_CONST ||
			token->token_type == TOKEN_STR_CONST ||
			token->token_type == TOKEN_TYPE
		) {
			push_ast_node(token, &last, AST_NODE_CONST, &node);
			continue;
		}

		free(head);
		PANIC(ERR_UNEXPECTED_TOKEN, { .tok = token });
	}

	if (!token && indent_lvl != 0) {
		free(head);
		PANIC(ERR_EOF_NO_BRACKET, {0});
	}

	if (node->last) {
		node->last->next = NULL;
		node->last = NULL;
		free(node);
	}
	return head; // NOLINT
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
	*last = token->next; // NOLINT

	(*node)->next = new_node;
	(*node) = new_node;
}

/*
Makes an AstNode with default values
*/
AstNode *make_ast_node(void) {
	AstNode *node;
	node = calloc(1, sizeof *node);
	DIE_IF_MALLOC_FAILS(node);

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
				free(ATTR(AstNodeFunctionProto, node, param_names));
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

	unsigned current_type = va_arg(vargs, unsigned); // NOLINT

	while (current_type != TOKEN_END) {
		if (token == NULL || token->token_type != current_type) {
			return false;
		}

		current_type = va_arg(vargs, unsigned); // NOLINT

		token = token->next;
	}

	va_end(vargs);
	return true;
}