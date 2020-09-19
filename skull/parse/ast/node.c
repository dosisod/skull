#include <stdbool.h>
#include <stdlib.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/parse/classify.h"

#include "skull/parse/ast/node.h"

#define MAKE_COMBO(name, ...) Combo name[] = { __VA_ARGS__, {0} }

MAKE_COMBO(ast_node_comment_combo,
	{ .tok = TOKEN_COMMENT }
);

MAKE_COMBO(ast_node_int_combo,
	{ .tok = TOKEN_INT_CONST }
);

MAKE_COMBO(ast_node_float_combo,
	{ .tok = TOKEN_FLOAT_CONST }
);

MAKE_COMBO(ast_node_bool_combo,
	{ .tok = TOKEN_BOOL_CONST }
);

MAKE_COMBO(ast_node_rune_combo,
	{ .tok = TOKEN_RUNE_CONST }
);

MAKE_COMBO(ast_node_str_combo,
	{ .tok = TOKEN_STR_CONST }
);

MAKE_COMBO(ast_node_identifier_combo,
	{ .tok = TOKEN_IDENTIFIER }
);

MAKE_COMBO(ast_node_type_combo,
	{ .tok = TOKEN_TYPE }
);

MAKE_COMBO(ast_node_var_combo,
	{ .tok = TOKEN_NEW_IDENTIFIER },
	{ .tok = TOKEN_TYPE },
	{ .tok = TOKEN_OPER_EQUAL }
);

MAKE_COMBO(ast_node_mut_var_def_combo,
	{ .tok = TOKEN_KW_MUT },
	{ .tok = TOKEN_NEW_IDENTIFIER },
	{ .tok = TOKEN_TYPE },
	{ .tok = TOKEN_OPER_EQUAL }
);

MAKE_COMBO(ast_node_auto_var_def_combo,
	{ .tok = TOKEN_IDENTIFIER },
	{ .tok = TOKEN_OPER_AUTO_EQUAL }
);

MAKE_COMBO(ast_node_mut_auto_var_def_combo,
	{ .tok = TOKEN_KW_MUT },
	{ .tok = TOKEN_IDENTIFIER },
	{ .tok = TOKEN_OPER_AUTO_EQUAL }
);

MAKE_COMBO(ast_node_var_assign_combo,
	{ .tok = TOKEN_IDENTIFIER },
	{ .tok = TOKEN_OPER_EQUAL }
);

MAKE_COMBO(ast_node_import_combo,
	{ .tok = TOKEN_KW_IMPORT },
	{ .tok = TOKEN_IDENTIFIER },
	{ .tok = TOKEN_KW_FROM },
	{ .tok = TOKEN_STR_CONST }
);

MAKE_COMBO(ast_node_add_var_combo,
	{ .tok = TOKEN_IDENTIFIER },
	{ .tok = TOKEN_OPER_PLUS },
	{ .tok = TOKEN_IDENTIFIER }
);

MAKE_COMBO(ast_node_sub_var_combo,
	{ .tok = TOKEN_IDENTIFIER },
	{ .tok = TOKEN_OPER_MINUS },
	{ .tok = TOKEN_IDENTIFIER }
);

MAKE_COMBO(ast_node_mult_var_combo,
	{ .tok = TOKEN_IDENTIFIER },
	{ .tok = TOKEN_OPER_MULT },
	{ .tok = TOKEN_IDENTIFIER }
);

MAKE_COMBO(ast_node_div_var_combo,
	{ .tok = TOKEN_IDENTIFIER },
	{ .tok = TOKEN_OPER_DIV },
	{ .tok = TOKEN_IDENTIFIER }
);

MAKE_COMBO(ast_node_return_combo,
	{ .tok = TOKEN_KW_RETURN },
	{ .tok = TOKEN_INT_CONST }
);

MAKE_COMBO(ast_node_return_var_combo,
	{ .tok = TOKEN_KW_RETURN },
	{ .tok = TOKEN_IDENTIFIER }
);

MAKE_COMBO(ast_node_no_param_func_combo,
	{ .tok = TOKEN_IDENTIFIER },
	{ .tok = TOKEN_BRACKET_OPEN },
	{ .tok = TOKEN_BRACKET_CLOSE }
);

MAKE_COMBO(ast_node_one_param_func_combo,
	{ .tok = TOKEN_IDENTIFIER },
	{ .tok = TOKEN_BRACKET_OPEN },
	{ .tok = TOKEN_ANY_NON_BRACKET_TOKEN },
	{ .tok = TOKEN_BRACKET_CLOSE }
);

MAKE_COMBO(ast_node_if_combo,
	{ .tok = TOKEN_KW_IF },
	{ .tok = TOKEN_BOOL_CONST }
);

#undef MAKE_COMBO

#define TRY_PUSH_AST_NODE(combo, node_type) \
	token = ast_token_cmp(token, (combo), &passed); \
	if (passed) { \
		passed = false; \
		push_ast_node(token, &last, (node_type), &node); \
		if (token->token_type == TOKEN_BRACKET_CLOSE) { \
			allow_top_lvl_bracket = true; \
		} \
		continue; \
	}

/*
Makes an AST (abstract syntax tree) from a given string.
*/
AstNode *make_ast_tree(const char32_t *code, const char32_t **error) {
	Token *token = tokenize(code);
	classify_tokens(token);

	return make_ast_tree_(token, error, 0);
}

/*
Internal AST tree generator.
*/
AstNode *make_ast_tree_(Token *token, const char32_t **error, unsigned indent_lvl) {
	Token *last = token;

	AstNode *node = make_ast_node();
	AstNode *head = node;
	bool passed = false;
	bool allow_top_lvl_bracket = false;

	while (token) {
		if (token->token_type == TOKEN_BRACKET_OPEN) {
			AstNode *child = make_ast_tree_(token->next, error, indent_lvl + 1);
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

			if (!child->token_end || !token->next) {
				return head;
			}
			push_ast_node(token->next, &last, 0, &node);
			node = node->last;
			token = child->token_end->next;

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
			node->last && node->last->token_end == token))
		{
			token = token->next;
			continue;
		}

		last = token;

		TRY_PUSH_AST_NODE(ast_node_var_combo, AST_NODE_VAR_DEF);
		TRY_PUSH_AST_NODE(ast_node_mut_var_def_combo, AST_NODE_MUT_VAR_DEF);
		TRY_PUSH_AST_NODE(ast_node_auto_var_def_combo, AST_NODE_AUTO_VAR_DEF);
		TRY_PUSH_AST_NODE(ast_node_mut_auto_var_def_combo, AST_NODE_MUT_AUTO_VAR_DEF);
		TRY_PUSH_AST_NODE(ast_node_var_assign_combo, AST_NODE_VAR_ASSIGN);
		TRY_PUSH_AST_NODE(ast_node_import_combo, AST_NODE_IMPORT);
		TRY_PUSH_AST_NODE(ast_node_add_var_combo, AST_NODE_ADD_VAR);
		TRY_PUSH_AST_NODE(ast_node_sub_var_combo, AST_NODE_SUB_VAR);
		TRY_PUSH_AST_NODE(ast_node_mult_var_combo, AST_NODE_MULT_VAR);
		TRY_PUSH_AST_NODE(ast_node_div_var_combo, AST_NODE_DIV_VAR);
		TRY_PUSH_AST_NODE(ast_node_return_combo, AST_NODE_RETURN);
		TRY_PUSH_AST_NODE(ast_node_return_var_combo, AST_NODE_RETURN);
		TRY_PUSH_AST_NODE(ast_node_no_param_func_combo, AST_NODE_NO_PARAM_FUNC);
		TRY_PUSH_AST_NODE(ast_node_one_param_func_combo, AST_NODE_ONE_PARAM_FUNC);
		TRY_PUSH_AST_NODE(ast_node_if_combo, AST_NODE_IF);
		TRY_PUSH_AST_NODE(ast_node_comment_combo, AST_NODE_COMMENT);
		TRY_PUSH_AST_NODE(ast_node_int_combo, AST_NODE_INT_CONST);
		TRY_PUSH_AST_NODE(ast_node_float_combo, AST_NODE_FLOAT_CONST);
		TRY_PUSH_AST_NODE(ast_node_bool_combo, AST_NODE_BOOL_CONST);
		TRY_PUSH_AST_NODE(ast_node_rune_combo, AST_NODE_RUNE_CONST);
		TRY_PUSH_AST_NODE(ast_node_str_combo, AST_NODE_STR_CONST);
		TRY_PUSH_AST_NODE(ast_node_identifier_combo, AST_NODE_IDENTIFIER);
		TRY_PUSH_AST_NODE(ast_node_type_combo, AST_NODE_TYPE_CONST);

		free(head);
		*error = FMT_ERROR(ERR_UNEXPECTED_TOKEN, { .tok = token });
		return NULL;
	}

	if (node->last) {
		node->last->next = NULL;
		node->last = NULL;
		free(node);
	}
	return head; // NOLINT
}

#undef TRY_PUSH_AST_NODE

/*
Compare tokens against a combonation of tokens.

Each item in `combo` will be compared with the next token after the last token.

For example:

```c
ast_token_cmp(token, (Combo[]){
    { .tok = 0 },
    { .tok = 1 },
    { .tok = 2 },
	{0}
});
```

will check up until `token->next->next`.

The last `{0}` is to tell the function to stop iterating.

If all the args match, return last token matched, else, the passed `token`.
*/
Token *ast_token_cmp(Token *token, Combo *combo, bool *pass) {
	Token *head = token;
	Token *last = head;

	while (token && (combo->tok || combo->combo)) {
		if (combo->rule == RULE_OPTIONAL) {
			if (combo->tok == token->token_type) {
				last = token;
				token = token->next;

				if (!token) {
					if (!combo->tok) {
						return head;
					}
					*pass = true;
					return last;
				}
			}
			combo++;
			continue;
		}
		if (combo->combo) {
			token = ast_token_cmp(token, combo->combo, pass);
			if (!*pass) {
				return head;
			}

			*pass = false;
		}
		else if ((token->token_type != combo->tok &&
			combo->tok != TOKEN_ANY_NON_BRACKET_TOKEN) || (
				combo->tok == TOKEN_ANY_NON_BRACKET_TOKEN &&
				(token->token_type == TOKEN_BRACKET_OPEN ||
				token->token_type == TOKEN_BRACKET_CLOSE)
			))
		{
			return head;
		}
		last = token;
		token = token->next;
		combo++;
	}

	if (!combo->tok || (combo->rule == RULE_OPTIONAL && !(combo + 1)->tok)) {
		*pass = true;
		return last;
	}
	return head;
}

/*
Push a new AST node to `node` with type `node_type`
*/
void push_ast_node(Token *token, Token **last, unsigned node_type, AstNode **node) {
	(*node)->node_type = node_type;
	(*node)->token = *last;
	(*node)->token_end = token;

	AstNode *tmp = make_ast_node();

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
	}
	free(node);
}