#include <stdlib.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/eval/eval_oper.h"
#include "skull/eval/types/bool.h"
#include "skull/eval/types/defs.h"
#include "skull/eval/types/float.h"
#include "skull/eval/types/int.h"
#include "skull/eval/types/rune.h"
#include "skull/eval/types/str.h"
#include "skull/parse/classify.h"

#include "skull/eval/eval_assign.h"

const char32_t *eval_auto_assign(Variable *const var, const AstNode *const node, const Scope *scope);

/*
Assign `node` to variable `var`.

Set `scope` to allow for assigning variables to other variables.

Return an error (as a string) if any occured, else `NULL`.
*/
const char32_t *eval_assign(Variable *const var, const AstNode *const node, const Scope *scope) {
	if (!node) {
		return FMT_ERROR(ERR_MISSING_ASSIGNMENT, { .var = var });
	}

	if (scope && node->node_type == AST_NODE_IDENTIFIER) {
		return eval_auto_assign(var, node, scope);
	}

	const void *mem = NULL;
	const char32_t *err = NULL;

	if (var->type == &TYPE_INT && node->node_type == AST_NODE_INT_CONST) {
		mem = eval_integer(node->token, &err);
	}
	else if (var->type == &TYPE_FLOAT && node->node_type == AST_NODE_FLOAT_CONST) {
		mem = eval_float(node->token, &err);
	}
	else if (var->type == &TYPE_BOOL && node->node_type == AST_NODE_BOOL_CONST) {
		mem = eval_bool(node->token, &err);
	}
	else if (var->type == &TYPE_RUNE && node->node_type == AST_NODE_RUNE_CONST) {
		mem = eval_rune(node->token, &err);
	}
	else if (var->type == &TYPE_STR && node->node_type == AST_NODE_STR_CONST) {
		SkullStr current = NULL;
		variable_read(&current, var);

		if (current) {
			free(current);
		}

		mem = eval_str(node->token, &err);
	}
	else {
		return FMT_ERROR(ERR_TYPE_MISMATCH, { .type = var->type });
	}

	if (err || (err = variable_write(var, mem)) == ERR_CANNOT_ASSIGN_CONST) {
		free((void *)mem);
		if (var->type == &TYPE_STR) {
			SkullStr str = NULL;
			variable_read(&str, var);
			free(str);
		}
		return err;
	}

	free((void *)mem);
	return NULL;
}

/*
Evaluate assignment via auto assignment operator.
*/
const char32_t *eval_auto_assign(Variable *const var, const AstNode *const node, const Scope *scope) {
	char32_t *const lookup = token_str(node->token);
	const Variable *const var_found = scope_find_name(scope, lookup);

	if (!var_found) {
		return FMT_ERROR(ERR_VAR_NOT_FOUND, { .real = lookup });
	}
	free(lookup);
	if (var_found->type != var->type) {
		return FMT_ERROR(ERR_TYPE_MISMATCH, { .type = var->type });
	}

	unsigned char *mem;
	mem = malloc(var_found->type->bytes * sizeof *mem);
	DIE_IF_MALLOC_FAILS(mem);

	variable_read(mem, var_found);

	if (var->type == &TYPE_STR) {
		SkullStr str = NULL;
		variable_read(&str, var_found);

		str = c32sdup(str);
		memcpy(mem, &str, sizeof str);
	}
	variable_write(var, mem);

	free(mem);
	return NULL;
}

#undef EVAL_ASSIGN_SETUP