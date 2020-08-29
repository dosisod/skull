#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/malloc.h"
#include "skull/parse/classify.h"

#include "skull/parse/ast/node.h"

int ast_node_var_combo[] = {
	TOKEN_NEW_IDENTIFIER,
	TOKEN_TYPE,
	TOKEN_OPER_EQUAL, -1
};

int ast_node_mut_var_def_combo[] = {
	TOKEN_KW_MUT,
	TOKEN_NEW_IDENTIFIER,
	TOKEN_TYPE,
	TOKEN_OPER_EQUAL, -1
};

int ast_node_auto_var_def_combo[] = {
	TOKEN_IDENTIFIER,
	TOKEN_OPER_AUTO_EQUAL, -1
};

int ast_node_mut_auto_var_def_combo[] = {
	TOKEN_KW_MUT,
	TOKEN_IDENTIFIER,
	TOKEN_OPER_AUTO_EQUAL, -1
};

int ast_node_var_assign_combo[] = {
	TOKEN_IDENTIFIER,
	TOKEN_OPER_EQUAL, -1
};

int ast_node_add_var_combo[] = {
	TOKEN_IDENTIFIER,
	TOKEN_OPER_PLUS,
	TOKEN_IDENTIFIER, -1
};

int ast_node_sub_var_combo[] = {
	TOKEN_IDENTIFIER,
	TOKEN_OPER_MINUS,
	TOKEN_IDENTIFIER, -1
};

int ast_node_mult_var_combo[] = {
	TOKEN_IDENTIFIER,
	TOKEN_OPER_MULT,
	TOKEN_IDENTIFIER, -1
};

int ast_node_div_var_combo[] = {
	TOKEN_IDENTIFIER,
	TOKEN_OPER_DIV,
	TOKEN_IDENTIFIER, -1
};

int ast_node_return_combo[] = {
	TOKEN_KW_RETURN,
	TOKEN_INT_CONST, -1
};

int ast_node_return_var_combo[] = {
	TOKEN_KW_RETURN,
	TOKEN_IDENTIFIER, -1
};

int ast_node_no_param_func_combo[] = {
	TOKEN_IDENTIFIER,
	TOKEN_BRACKET_OPEN,
	TOKEN_BRACKET_CLOSE, -1
};

int ast_node_one_param_func_combo[] = {
	TOKEN_IDENTIFIER,
	TOKEN_BRACKET_OPEN,
	TOKEN_ANY_NON_BRACKET_TOKEN,
	TOKEN_BRACKET_CLOSE, -1
};

int ast_node_if_combo[] = {
	TOKEN_KW_IF,
	TOKEN_BOOL_CONST,
	TOKEN_BRACKET_OPEN,
		//copied from return_combo
		TOKEN_KW_RETURN,
		TOKEN_INT_CONST,
	TOKEN_BRACKET_CLOSE, -1
};

#define TRY_PUSH_AST_NODE(combo, node_type) \
	token = ast_token_cmp(token, (combo)); \
	if (token != last) { \
		push_ast_node(token, &last, (node_type), &node); \
		continue; \
	}

#define PUSH_NODE_IF_TOKEN(tok_type, node_type) \
	else if (token->token_type == (tok_type)) { \
		push_ast_node(token, &last, (node_type), &node); \
	}

/*
Makes an AST (abstract syntax tree) from a given string.
*/
ast_node_t *make_ast_tree(const char32_t *code) {
	token_t *token = tokenize(code);
	token_t *last;

	classify_tokens(token);

	ast_node_t *node = make_ast_node();
	ast_node_t *head = node;

	while (token) {
		last = token;

		TRY_PUSH_AST_NODE(ast_node_var_combo, AST_NODE_VAR_DEF);
		TRY_PUSH_AST_NODE(ast_node_mut_var_def_combo, AST_NODE_MUT_VAR_DEF);
		TRY_PUSH_AST_NODE(ast_node_auto_var_def_combo, AST_NODE_AUTO_VAR_DEF);
		TRY_PUSH_AST_NODE(ast_node_mut_auto_var_def_combo, AST_NODE_MUT_AUTO_VAR_DEF);
		TRY_PUSH_AST_NODE(ast_node_var_assign_combo, AST_NODE_VAR_ASSIGN);
		TRY_PUSH_AST_NODE(ast_node_add_var_combo, AST_NODE_ADD_VAR);
		TRY_PUSH_AST_NODE(ast_node_sub_var_combo, AST_NODE_SUB_VAR);
		TRY_PUSH_AST_NODE(ast_node_mult_var_combo, AST_NODE_MULT_VAR);
		TRY_PUSH_AST_NODE(ast_node_div_var_combo, AST_NODE_DIV_VAR);
		TRY_PUSH_AST_NODE(ast_node_return_combo, AST_NODE_RETURN);
		TRY_PUSH_AST_NODE(ast_node_return_var_combo, AST_NODE_RETURN);
		TRY_PUSH_AST_NODE(ast_node_no_param_func_combo, AST_NODE_NO_PARAM_FUNC);
		TRY_PUSH_AST_NODE(ast_node_one_param_func_combo, AST_NODE_ONE_PARAM_FUNC);
		TRY_PUSH_AST_NODE(ast_node_if_combo, AST_NODE_IF);

		//skip to next token if current token is present in last node
		if (node->last && node->last->token_end == token) {
			token = token->next;
			continue;
		}

		if (false) {} // setup for macros

		PUSH_NODE_IF_TOKEN(TOKEN_COMMENT, AST_NODE_COMMENT)
		PUSH_NODE_IF_TOKEN(TOKEN_INT_CONST, AST_NODE_INT_CONST)
		PUSH_NODE_IF_TOKEN(TOKEN_FLOAT_CONST, AST_NODE_FLOAT_CONST)
		PUSH_NODE_IF_TOKEN(TOKEN_BOOL_CONST, AST_NODE_BOOL_CONST)
		PUSH_NODE_IF_TOKEN(TOKEN_RUNE_CONST, AST_NODE_RUNE_CONST)
		PUSH_NODE_IF_TOKEN(TOKEN_STR_CONST, AST_NODE_STR_CONST)
		PUSH_NODE_IF_TOKEN(TOKEN_IDENTIFIER, AST_NODE_IDENTIFIER)
		PUSH_NODE_IF_TOKEN(TOKEN_TYPE, AST_NODE_TYPE_CONST)

		else {
			break;
		}

		token = token->next;
	}

	if (node->last) {
		node->last->next = NULL;
		node->last = NULL;
		free(node);
	}
	return head;
}

#undef TRY_PUSH_AST_NODE
#undef PUSH_NODE_IF_TOKEN

/*
Compare tokens against a combonation of tokens.

Each item in `combo` will be compared with the next token after the last token.

For example, `ast_token_cmp(token, (int[]){0, 1, 2, -1})` will check up until `token->next->next`.

The last `-1` is to tell the function to stop iterating.

If all the args match, return last token matched, else, the passed `token`.
*/
token_t *ast_token_cmp(token_t *token, int *token_type) {
	token_t *head = token;
	token_t *last = head;

	while (token && *token_type != -1) {
		if (
			token->token_type != *token_type &&
			*token_type != TOKEN_ANY_NON_BRACKET_TOKEN)
		{
			return head;
		}
		if (*token_type == TOKEN_ANY_NON_BRACKET_TOKEN &&
			(token->token_type == TOKEN_BRACKET_OPEN ||
			token->token_type == TOKEN_BRACKET_CLOSE))
		{
			return head;
		}
		last = token;
		token = token->next;
		token_type++;
	}

	if (*token_type == -1) {
		return last;
	}
	return head;
}

/*
Push a new AST node to `node` with type `node_type`
*/
void push_ast_node(token_t *token, token_t **last, uint8_t node_type, ast_node_t **node) {
	(*node)->node_type = node_type;
	(*node)->token = *last;
	(*node)->token_end = token;

	ast_node_t *tmp = make_ast_node();

	tmp->last = *node;
	*last = token->next;

	(*node)->next = tmp;
	(*node) = tmp;
}

/*
Makes an ast_node_t with default values
*/
ast_node_t *make_ast_node(void) {
	ast_node_t *node;
	node = calloc(1, sizeof *node);
	DIE_IF_MALLOC_FAILS(node);

	return node;
}

/*
Frees an AST tree.
*/
void free_ast_tree(ast_node_t *node) {
	free_tokens(node->token);
	free(node);
}