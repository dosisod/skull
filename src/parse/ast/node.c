#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "../classify.h"
#include "../../common/malloc.h"

#include "node.h"

/*
Makes an ast_node_t with default values
*/
ast_node_t *make_ast_node(void) {
	ast_node_t *node=malloc(sizeof(ast_node_t));
	DIE_IF_MALLOC_FAILS(node);

	node->node_type=AST_NODE_UNKNOWN;
	node->token=NULL;
	node->token_end=NULL;
	node->last=NULL;
	node->next=NULL;

	return node;
}

/*
Compare tokens against a combonation of tokens.

Each item in `combo` will be compared with the next token after the last token.

For example, `ast_token_cmp(token, (int[]){0, 1, 2, -1})` will check up until `token->next->next`.

The last `-1` is to tell the function to stop iterating.

If all the args match, return last token matched, else, the passed `token`.
*/
token_t *ast_token_cmp(token_t *token, int *token_type) {
	token_t *head=token;
	token_t *last=head;

	while (token!=NULL && *token_type!=-1) {
		if (token->token_type!=*token_type && *token_type!=TOKEN_ANY_NON_BRACKET_TOKEN) {
			return head;
		}
		if (*token_type==TOKEN_ANY_NON_BRACKET_TOKEN &&
			(token->token_type==TOKEN_BRACKET_OPEN ||
			token->token_type==TOKEN_BRACKET_CLOSE))
		{
			return head;
		}
		last=token;
		token=token->next;
		token_type++;
	}

	if (*token_type==-1) {
		return last;
	}
	return head;
}

/*
Push a new AST node to `node` with type `node_type`
*/
void push_ast_node(token_t *token, token_t **last, uint8_t node_type, ast_node_t **node) {
	(*node)->node_type=node_type;
	(*node)->token=(*last);
	(*node)->token_end=token;

	ast_node_t *tmp=make_ast_node();

	tmp->last=*node;
	*last=token->next;

	(*node)->next=tmp;
	(*node)=tmp;
}

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

#define TRY_PUSH_AST_NODE(combo, node_type) \
token=ast_token_cmp(token, (combo)); \
if (token!=last) { \
	push_ast_node(token, &last, (node_type), &node); \
	continue; \
}

/*
Makes an AST (abstract syntax tree) from a given string.
*/
ast_node_t *make_ast_tree(const char32_t *code) {
	token_t *token=tokenize(code);
	token_t *last;

	classify_tokens(token);

	ast_node_t *node=make_ast_node();
	ast_node_t *head=node;

	while (token!=NULL) {
		last=token;

		TRY_PUSH_AST_NODE(ast_node_var_combo, AST_NODE_VAR_DEF);
		TRY_PUSH_AST_NODE(ast_node_mut_var_def_combo, AST_NODE_MUT_VAR_DEF);
		TRY_PUSH_AST_NODE(ast_node_auto_var_def_combo, AST_NODE_AUTO_VAR_DEF);
		TRY_PUSH_AST_NODE(ast_node_mut_auto_var_def_combo, AST_NODE_MUT_AUTO_VAR_DEF);
		TRY_PUSH_AST_NODE(ast_node_var_assign_combo, AST_NODE_VAR_ASSIGN);
		TRY_PUSH_AST_NODE(ast_node_add_var_combo, AST_NODE_ADD_VAR);
		TRY_PUSH_AST_NODE(ast_node_return_combo, AST_NODE_RETURN);
		TRY_PUSH_AST_NODE(ast_node_return_var_combo, AST_NODE_RETURN);
		TRY_PUSH_AST_NODE(ast_node_no_param_func_combo, AST_NODE_NO_PARAM_FUNC);
		TRY_PUSH_AST_NODE(ast_node_one_param_func_combo, AST_NODE_ONE_PARAM_FUNC);

		//skip to next token if current token is present in last node
		if (node->last && node->last->token_end==token) {
			token=token->next;
			continue;
		}
		if (token->token_type==TOKEN_COMMENT) {
			push_ast_node(token, &last, AST_NODE_COMMENT, &node);
		}
		else if (token->token_type==TOKEN_INT_CONST) {
			push_ast_node(token, &last, AST_NODE_INT_CONST, &node);
		}
		else if (token->token_type==TOKEN_FLOAT_CONST) {
			push_ast_node(token, &last, AST_NODE_FLOAT_CONST, &node);
		}
		else if (token->token_type==TOKEN_BOOL_CONST) {
			push_ast_node(token, &last, AST_NODE_BOOL_CONST, &node);
		}
		else if (token->token_type==TOKEN_CHAR_CONST) {
			push_ast_node(token, &last, AST_NODE_CHAR_CONST, &node);
		}
		else if (token->token_type==TOKEN_STR_CONST) {
			push_ast_node(token, &last, AST_NODE_STR_CONST, &node);
		}
		else {
			break;
		}
		token=token->next;
	}

	if (node->last!=NULL) {
		node->last->next=NULL;
		node->last=NULL;
		free(node);
	}
	return head;
}

/*
Frees an AST tree.
*/
void free_ast_tree(ast_node_t *node) {
	free_tokens(node->token);
	free(node);
}