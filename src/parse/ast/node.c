#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "../classify.h"
#include "../../common/malloc.h"

#include "node.h"

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

token_t *ast_token_cmp(token_t *token, ...) {
	token_t *head=token;
	token_t *last=head;

	va_list args;
	va_start(args, token);

	int token_type=va_arg(args, int); // NOLINT

	while (token!=NULL && token_type!=-1) {
		if (token->token_type!=token_type && token_type!=TOKEN_ANY_NON_BRACKET_TOKEN) {
			return head;
		}
		if (token_type==TOKEN_ANY_NON_BRACKET_TOKEN &&
			(token->token_type==TOKEN_BRACKET_OPEN ||
			token->token_type==TOKEN_BRACKET_CLOSE))
		{
			return head;
		}
		token_type=va_arg(args, int);
		last=token;
		token=token->next;
	}

	va_end(args);

	if (token_type==-1) {
		return last;
	}
	return head;
}

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

#define PUSH_AST_NODE_IF(token, last, node_type, node) \
if ((token)!=*(last)) { \
	push_ast_node((token), (last), (node_type), (node)); \
	continue; \
} \

ast_node_t *make_ast_tree(const char32_t *code) {
	token_t *token=tokenize(code);
	token_t *last;

	classify_tokens(token);

	ast_node_t *node=make_ast_node();
	ast_node_t *head=node;

	while (token!=NULL) {
		last=token;
		token=ast_token_cmp(token,
			TOKEN_NEW_IDENTIFIER,
			TOKEN_TYPE,
			TOKEN_OPER_EQUAL, -1
		);
		PUSH_AST_NODE_IF(token, &last, AST_NODE_VAR_DEF, &node);

		token=ast_token_cmp(token,
			TOKEN_KW_MUT,
			TOKEN_NEW_IDENTIFIER,
			TOKEN_TYPE,
			TOKEN_OPER_EQUAL, -1
		);
		PUSH_AST_NODE_IF(token, &last, AST_NODE_MUT_VAR_DEF, &node);

		token=ast_token_cmp(token,
			TOKEN_IDENTIFIER,
			TOKEN_OPER_AUTO_EQUAL, -1
		);
		PUSH_AST_NODE_IF(token, &last, AST_NODE_AUTO_VAR_DEF, &node);

		token=ast_token_cmp(token,
			TOKEN_KW_MUT,
			TOKEN_IDENTIFIER,
			TOKEN_OPER_AUTO_EQUAL, -1
		);
		PUSH_AST_NODE_IF(token, &last, AST_NODE_MUT_AUTO_VAR_DEF, &node);

		token=ast_token_cmp(token,
			TOKEN_IDENTIFIER,
			TOKEN_OPER_EQUAL, -1
		);
		PUSH_AST_NODE_IF(token, &last, AST_NODE_VAR_ASSIGN, &node);

		token=ast_token_cmp(token,
			TOKEN_IDENTIFIER,
			TOKEN_OPER_PLUS,
			TOKEN_IDENTIFIER, -1
		);
		PUSH_AST_NODE_IF(token, &last, AST_NODE_ADD_VAR, &node);

		token=ast_token_cmp(token,
			TOKEN_KW_RETURN,
			TOKEN_INT_CONST, -1
		);
		PUSH_AST_NODE_IF(token, &last, AST_NODE_RETURN, &node);

		token=ast_token_cmp(token,
			TOKEN_IDENTIFIER,
			TOKEN_BRACKET_OPEN,
			TOKEN_BRACKET_CLOSE, -1
		);
		PUSH_AST_NODE_IF(token, &last, AST_NODE_NO_PARAM_FUNC, &node);

		token=ast_token_cmp(token,
			TOKEN_IDENTIFIER,
			TOKEN_BRACKET_OPEN,
			TOKEN_ANY_NON_BRACKET_TOKEN,
			TOKEN_BRACKET_CLOSE, -1
		);
		PUSH_AST_NODE_IF(token, &last, AST_NODE_ONE_PARAM_FUNC, &node);

		//skip to next token if current token is present in last node
		if (node->last && node->last->token_end==token) {
			token=token->next;
			continue;
		}
		if (token->token_type==TOKEN_INT_CONST) {
			push_ast_node(token, &last, AST_NODE_INT_CONST, &node);
		}
		if (token->token_type==TOKEN_FLOAT_CONST) {
			push_ast_node(token, &last, AST_NODE_FLOAT_CONST, &node);
		}
		if (token->token_type==TOKEN_BOOL_CONST) {
			push_ast_node(token, &last, AST_NODE_BOOL_CONST, &node);
		}
		if (token->token_type==TOKEN_CHAR_CONST) {
			push_ast_node(token, &last, AST_NODE_CHAR_CONST, &node);
		}
		if (token->token_type==TOKEN_STR_CONST) {
			push_ast_node(token, &last, AST_NODE_STR_CONST, &node);
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

void free_ast_tree(ast_node_t *node) {
	free_tokens(node->token);
	free(node);
}