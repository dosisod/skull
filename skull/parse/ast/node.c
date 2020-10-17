#include <stdbool.h>
#include <stdlib.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/parse/classify.h"

#include "skull/parse/ast/node.h"

/*
Makes an AST (abstract syntax tree) from a given string.
*/
AstNode *make_ast_tree(const char32_t *const code, char32_t **error) {
	Token *const token = tokenize(code);
	classify_tokens(token);

	AstNode *const ret = make_ast_tree_(token, error, 0);
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

	if (token->token_type == TOKEN_NEW_IDENTIFIER &&
		token->next &&
		token->next->token_type == TOKEN_TYPE &&
		token->next->next &&
		token->next->next->token_type == TOKEN_OPER_EQUAL
	) {
		is_implicit = false;
		*_token = token->next->next;
	}

	else if (token->token_type == TOKEN_IDENTIFIER &&
		token->next &&
		token->next->token_type == TOKEN_OPER_AUTO_EQUAL
	) {
		*_token = token->next;
	}

	else {
		return false;
	}

	AstNodeVarDef *attr;
	attr = malloc(sizeof *attr);
	DIE_IF_MALLOC_FAILS(attr);

	*attr = (AstNodeVarDef){
		.is_const = is_const,
		.is_implicit = is_implicit
	};

	(*node)->attr = attr;

	push_ast_node(*_token, last, AST_NODE_VAR_DEF, node);
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
AstNode *make_ast_tree_(Token *token, char32_t **error, unsigned indent_lvl) {
	Token *last = token;

	AstNode *node = make_ast_node();
	AstNode *head = node;
	bool allow_top_lvl_bracket = false;

	while (token) {
		if (token->token_type == TOKEN_BRACKET_OPEN) {
			AstNode *const child = make_ast_tree_(token->next, error, indent_lvl + 1);
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

			// TODO(x): return the last token that was reached so we dont have to do this
			while (token->token_type != TOKEN_BRACKET_CLOSE) {
				if (!token->next) {
					*error = FMT_ERROR(ERR_UNEXPECTED_TOKEN, { .tok = node->token });
					return NULL;
				}
				token = token->next;
			}
			token = token->next;
			continue;
		}

		if (token->token_type == TOKEN_BRACKET_CLOSE) {
			if (indent_lvl == 0 && !allow_top_lvl_bracket) {
				free(head);
				*error = FMT_ERROR(ERR_UNEXPECTED_TOKEN, { .tok = token });
				return NULL;
			}

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

		if (token->token_type == TOKEN_IDENTIFIER &&
			token->next &&
			token->next->token_type == TOKEN_OPER_EQUAL
		) {
			token = token->next;
			push_ast_node(token, &last, AST_NODE_VAR_ASSIGN, &node);
			continue;
		}

		if (token->token_type == TOKEN_KW_IMPORT &&
			token->next &&
			token->next->token_type == TOKEN_IDENTIFIER &&
			token->next->next &&
			token->next->next->token_type == TOKEN_KW_FROM &&
			token->next->next->next &&
			token->next->next->next->token_type == TOKEN_STR_CONST
		) {
			token = token->next->next->next;
			push_ast_node(token, &last, AST_NODE_IMPORT, &node);
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
			if (token->token_type == TOKEN_BRACKET_CLOSE) {
				allow_top_lvl_bracket = true;
			}
			continue;
		}

		if (is_const_literal(token) &&
			token->next &&
			token->next->token_type == TOKEN_OPER_PLUS &&
			token->next->next &&
			is_const_literal(token->next->next)
		) {
			token = token->next->next;
			push_ast_node(token, &last, AST_NODE_ADD_CONSTS, &node);
			continue;
		}

		if (is_const_literal(token) &&
			token->next &&
			token->next->token_type == TOKEN_OPER_MINUS &&
			token->next->next &&
			is_const_literal(token->next->next)
		) {
			token = token->next->next;
			push_ast_node(token, &last, AST_NODE_SUB_CONSTS, &node);
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
		if (token->token_type == TOKEN_INT_CONST) {
			push_ast_node(token, &last, AST_NODE_INT_CONST, &node);
			continue;
		}
		if (token->token_type == TOKEN_FLOAT_CONST) {
			push_ast_node(token, &last, AST_NODE_FLOAT_CONST, &node);
			continue;
		}
		if (token->token_type == TOKEN_BOOL_CONST) {
			push_ast_node(token, &last, AST_NODE_BOOL_CONST, &node);
			continue;
		}
		if (token->token_type == TOKEN_RUNE_CONST) {
			push_ast_node(token, &last, AST_NODE_RUNE_CONST, &node);
			continue;
		}
		if (token->token_type == TOKEN_STR_CONST) {
			push_ast_node(token, &last, AST_NODE_STR_CONST, &node);
			continue;
		}
		if (token->token_type == TOKEN_TYPE) {
			push_ast_node(token, &last, AST_NODE_TYPE_CONST, &node);
			continue;
		}

		free(head);
		*error = FMT_ERROR(ERR_UNEXPECTED_TOKEN, { .tok = token });
		return NULL;
	}

	if (!token && indent_lvl != 0) {
		free(head);
		*error = c32sdup(ERR_EOF_NO_BRACKET);
		return NULL;
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

	AstNode *const tmp = make_ast_node();

	tmp->last = *node;
	*last = token->next;

	(*node)->next = tmp;
	(*node) = tmp;
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
	AstNode *tmp = NULL;

	while (node) {
		if (node->attr) {
			free(node->attr);
		}

		if (node->child) {
			free_ast_tree_(node->child);
		}

		tmp = node;
		node = node->next;
		tmp->next = NULL;
		free(tmp);
	}
}