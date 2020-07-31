#include <stdlib.h>
#include <string.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/eval/eval_add.h"
#include "skull/eval/eval_div.h"
#include "skull/eval/eval_float.h"
#include "skull/eval/eval_integer.h"
#include "skull/eval/eval_mult.h"
#include "skull/eval/eval_sub.h"
#include "skull/eval/types/bool.h"
#include "skull/eval/types/char.h"
#include "skull/eval/types/str.h"
#include "skull/eval/types/type.h"
#include "skull/parse/classify.h"

#include "skull/eval/eval_assign.h"

const char32_t *eval_auto_assign(variable_t *var, ast_node_t *node, const context_t *ctx);

#define EVAL_ASSIGN_SETUP(func, cannot, unavail) \
	char32_t *lhs_str=token_str(node->token); \
	char32_t *rhs_str=token_str(node->token->next->next); \
	variable_t *lhs_var=context_find_name(ctx, lhs_str); \
	variable_t *rhs_var=context_find_name(ctx, rhs_str); \
	free(lhs_str); \
	free(rhs_str); \
	if (lhs_var==NULL || rhs_var==NULL) { \
		return ERR_VAR_NOT_FOUND; \
	} \
	if (lhs_var->type!=rhs_var->type) { \
		return (cannot); \
	} \
	variable_t *tmp=(func)(lhs_var, rhs_var); \
	if (tmp==NULL) { \
		return (unavail); \
	} \
	variable_write(var, tmp->mem); \
	free(tmp); \
	return NULL; \

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
		EVAL_ASSIGN_SETUP(eval_add, ERR_CANNOT_ADD, ERR_ADD_UNAVAILABLE);
	}

	if (ctx!=NULL && node->node_type==AST_NODE_SUB_VAR) {
		EVAL_ASSIGN_SETUP(eval_sub, ERR_CANNOT_SUB, ERR_SUB_UNAVAILABLE);
	}

	if (ctx!=NULL && node->node_type==AST_NODE_MULT_VAR) {
		EVAL_ASSIGN_SETUP(eval_mult, ERR_CANNOT_MULT, ERR_MULT_UNAVAILABLE);
	}

	if (ctx!=NULL && node->node_type==AST_NODE_DIV_VAR) {
		EVAL_ASSIGN_SETUP(eval_div, ERR_CANNOT_DIV, ERR_DIV_UNAVAILABLE);
	}

	const void *mem=NULL;
	const char32_t *err=NULL;

	if (var->type==&TYPE_INT && node->node_type==AST_NODE_INT_CONST) {
		mem=eval_integer(node->token, &err);
	}
	else if (var->type==&TYPE_FLOAT && node->node_type==AST_NODE_FLOAT_CONST) {
		mem=eval_float(node->token, &err);
	}
	else if (var->type==&TYPE_BOOL && node->node_type==AST_NODE_BOOL_CONST) {
		mem=eval_bool(node->token, &err);
	}
	else if (var->type==&TYPE_CHAR && node->node_type==AST_NODE_CHAR_CONST) {
		mem=eval_char(node->token, &err);
	}
	else if (var->type==&TYPE_STR && node->node_type==AST_NODE_STR_CONST) {
		char32_t *current=NULL;
		variable_read(&current, var);

		if (current!=NULL) {
			free(current);
		}

		mem=eval_str(node->token, &err);
	}
	else if (var->type==&TYPE_TYPE && node->node_type==AST_NODE_TYPE_CONST) {
		mem=eval_type(node->token, &err);
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

/*
Evaluate assignment via auto assignment operator.
*/
const char32_t *eval_auto_assign(variable_t *var, ast_node_t *node, const context_t *ctx) {
	char32_t *lookup=token_str(node->token);
	variable_t *var_found=context_find_name(ctx, lookup);
	free(lookup);

	if (var_found==NULL) {
		return ERR_VAR_NOT_FOUND;
	}
	if (var_found->type!=var->type) {
		return ERR_TYPE_MISMATCH;
	}

	uint8_t *mem=malloc(var_found->bytes * sizeof(uint8_t));
	variable_read(mem, var_found);

	if (var->type==&TYPE_STR) {
		char32_t *str=NULL;
		variable_read(&str, var_found);

		str=c32sdup(str);
		memcpy(mem, &str, sizeof(char32_t*));
	}
	variable_write(var, mem);

	free(mem);
	return NULL;
}

#undef EVAL_ASSIGN_SETUP