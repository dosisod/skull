#include <stdlib.h>
#include <string.h>

#include "../common/malloc.h"
#include "../common/str.h"
#include "../errors.h"
#include "../parse/classify.h"
#include "eval_add.h"
#include "eval_float.h"
#include "eval_integer.h"

#include "eval_assign.h"

#define SETUP_MEM(name, type, func) \
	const type tmp=(func); \
	mem=malloc(sizeof(type)); \
	DIE_IF_MALLOC_FAILS(mem); \
	memcpy((void*)mem, &tmp, sizeof(type));

/*
Evaluate assignment via auto assignment operator.
*/
const char32_t *eval_auto_assign(variable_t *var, ast_node_t *node, const context_t *ctx) {
	MAKE_TOKEN_BUF(buf, node->token);
	variable_t *var_found=context_find_name(ctx, buf);

	if (var_found==NULL) {
		return ERR_VAR_NOT_FOUND;
	}
	if (var_found->type!=var->type) {
		return ERR_TYPE_MISMATCH;
	}

	uint8_t mem[var_found->bytes];
	variable_read(mem, var_found);

	if (var->type==&TYPE_STR) {
		char32_t *str=NULL;
		variable_read(&str, var_found);

		str=c32sdup(str);
		memcpy(mem, &str, sizeof(char32_t*));
	}
	variable_write(var, mem);

	return NULL;
}

/*
Evaluate assignment via adding of 2 variables.
*/
const char32_t *eval_add_var_assign(variable_t *var, ast_node_t *node, const context_t *ctx) {
	MAKE_TOKEN_BUF(lhs_buf, node->token);
	MAKE_TOKEN_BUF(rhs_buf, node->token->next->next);

	variable_t *lhs_var=context_find_name(ctx, lhs_buf);
	variable_t *rhs_var=context_find_name(ctx, rhs_buf);

	if (lhs_var==NULL || rhs_var==NULL) {
		return ERR_VAR_NOT_FOUND;
	}
	if (lhs_var->type!=rhs_var->type) {
		return ERR_TYPE_MISMATCH;
	}

	variable_t *tmp=eval_add(lhs_var, rhs_var);
	if (tmp==NULL) {
		return ERR_TYPE_MISMATCH;
	}
	variable_write(var, tmp->mem);
	free(tmp);

	return NULL;
}

/*
Assign `node` to variable `var`.

Set `ctx` to allow for assigning variables to other variables.

Return an error (as a string) if any occured, else `NULL`.
*/
const char32_t *eval_assign(variable_t *var, ast_node_t *node, const context_t *ctx) {
	if (node==NULL) {
		return ERR_INVALID_INPUT;
	}

	if (ctx!=NULL && node->node_type==AST_NODE_IDENTIFIER) {
		return eval_auto_assign(var, node, ctx);
	}

	if (ctx!=NULL && node->node_type==AST_NODE_ADD_VAR) {
		return eval_add_var_assign(var, node, ctx);
	}

	const void *mem=NULL;
	const char32_t *err=NULL;

	if (var->type==&TYPE_INT && node->node_type==AST_NODE_INT_CONST) {
		SETUP_MEM(mem, int64_t, eval_integer(node->token, &err));
	}
	else if (var->type==&TYPE_FLOAT && node->node_type==AST_NODE_FLOAT_CONST) {
		SETUP_MEM(mem, long double, eval_float(node->token, &err));
	}
	else if (var->type==&TYPE_BOOL && node->node_type==AST_NODE_BOOL_CONST) {
		SETUP_MEM(mem, bool, token_cmp(U"true", node->token));
	}
	else if (var->type==&TYPE_CHAR && node->node_type==AST_NODE_CHAR_CONST) {
		SETUP_MEM(mem, char32_t, *node->token->begin);
	}
	else if (var->type==&TYPE_STR && node->node_type==AST_NODE_STR_CONST) {
		char32_t *current=NULL;
		variable_read(&current, var);

		if (current!=NULL) {
			free(current);
		}

		MAKE_TOKEN_BUF(buf, node->token);
		SETUP_MEM(mem, char32_t*, c32sdup(buf));
	}
	else if (var->type==&TYPE_TYPE && node->node_type==AST_NODE_TYPE_CONST) {
		MAKE_TOKEN_BUF(buf, node->token);
		const type_t *type=find_type(buf);

		if (type==&TYPE_TYPE) {
			return ERR_TYPE_MISMATCH;
		}

		SETUP_MEM(mem, type_t*, type);
	}
	else {
		return ERR_TYPE_MISMATCH;
	}

	if (err!=NULL || (err=variable_write(var, mem))==ERR_CANNOT_ASSIGN_CONST) {
		free((void*)mem);
		if (var->type==&TYPE_STR) {
			char32_t *str=NULL;
			variable_read(&str, var);
			free(str);
		}
		return err;
	}

	if (var->type!=&TYPE_STR) {
		free((void*)mem);
	}
	return NULL; // NOLINT
}

#undef SETUP_MEM