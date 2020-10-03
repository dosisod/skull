#include <stdio.h>
#include <stdlib.h>

#include <llvm-c/Core.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/eval/eval_assign.h"
#include "skull/eval/types/defs.h"
#include "skull/parse/classify.h"

#include "skull/llvm/aliases.h"

#include "skull/llvm/var.h"

/*
Convert a Skull variable `var` into the LLVM IR equivalent.
*/
void var_to_llvm_ir(Variable *var, LLVMBuilderRef builder, LLVMContextRef ctx) {
	char *var_name = c32stombs(var->name);

	if (var->type == &TYPE_INT) {
		if (!var->alloca) {
			var->alloca = LLVMBuildAlloca(
				builder,
				LLVMInt64TypeInContext(ctx),
				var_name
			);
		}

		SkullInt num = 0;
		variable_read(&num, var);

		LLVMBuildStore(
			builder,
			LLVM_INT(ctx, num),
			var->alloca
		);
	}
	if (var->type == &TYPE_FLOAT) {
		if (!var->alloca) {
			var->alloca = LLVMBuildAlloca(
				builder,
				LLVMDoubleTypeInContext(ctx),
				var_name
			);
		}

		SkullFloat num = 0;
		variable_read(&num, var);

		LLVMBuildStore(
			builder,
			LLVM_FLOAT(ctx, num),
			var->alloca
		);
	}
	else if (var->type == &TYPE_BOOL) {
		if (!var->alloca) {
			var->alloca = LLVMBuildAlloca(
				builder,
				LLVMInt1TypeInContext(ctx),
				var_name
			);
		}

		bool val = false;
		variable_read(&val, var);

		LLVMBuildStore(
			builder,
			LLVM_BOOL(ctx, val),
			var->alloca
		);
	}
	else if (var->type == &TYPE_RUNE) {
		if (!var->alloca) {
			var->alloca = LLVMBuildAlloca(
				builder,
				LLVMInt32TypeInContext(ctx),
				var_name
			);
		}

		char32_t c = U'\0';
		variable_read(&c, var);

		LLVMBuildStore(
			builder,
			LLVM_RUNE(ctx, c),
			var->alloca
		);
	}
	else if (var->type == &TYPE_STR) {
		char32_t *str = NULL;
		variable_read(&str, var);
		const size_t len = c32slen(str) + 1;

		LLVMValueRef *llvm_arr = malloc(sizeof(LLVMValueRef) * len);
		DIE_IF_MALLOC_FAILS(llvm_arr);

		size_t counter = 0;
		while (counter < len) {
			llvm_arr[counter] = LLVM_RUNE(ctx, str[counter]);
			counter++;
		}

		LLVMTypeRef str_type = LLVMArrayType(
			LLVMInt32TypeInContext(ctx),
			(unsigned)len
		);

		if (!var->alloca) {
			var->alloca = LLVMBuildAlloca(
				builder,
				str_type,
				var_name
			);
		}

		LLVMValueRef char_arr = LLVMConstArray(
			str_type,
			llvm_arr,
			(unsigned)len
		);
		free(llvm_arr);

		LLVMBuildStore(
			builder,
			char_arr,
			var->alloca
		);
	}

	free(var_name);
}

/*
Make and add a variable from `node` to `scope`.

Returns pointer to error message if one occurs, else `NULL`.
*/
const char32_t *node_make_var(const AstNode *node, Scope *scope) {
	if (!scope) {
		return NULL;
	}

	const Token *token = node->token;
	const bool is_const = ATTR(AstNodeVarDef, node, is_const);
	if (!is_const) {
		token = token->next;
	}

	if (!node->next) {
		return FMT_ERROR(ERR_MISSING_ASSIGNMENT, { .tok = token });
	}

	char32_t *name = token_str(token);
	Variable *var = NULL;

	if (ATTR(AstNodeVarDef, node, is_implicit)) {
		const Type *type = NULL;
		if (node->next->node_type == AST_NODE_INT_CONST) {
			type = &TYPE_INT;
		}
		else if (node->next->node_type == AST_NODE_FLOAT_CONST) {
			type = &TYPE_FLOAT;
		}
		else if (node->next->node_type == AST_NODE_BOOL_CONST) {
			type = &TYPE_BOOL;
		}
		else if (node->next->node_type == AST_NODE_STR_CONST) {
			type = &TYPE_STR;
		}
		else if (node->next->node_type == AST_NODE_RUNE_CONST) {
			type = &TYPE_RUNE;
		}
		else if (node->next->node_type == AST_NODE_IDENTIFIER ||
			node->next->node_type == AST_NODE_ADD_VAR ||
			node->next->node_type == AST_NODE_SUB_VAR ||
			node->next->node_type == AST_NODE_MULT_VAR ||
			node->next->node_type == AST_NODE_DIV_VAR
		) {
			char32_t *lookup = token_str(node->next->token);
			Variable *new_var = scope_find_name(scope, lookup);

			if (!new_var) {
				free(name);
				return FMT_ERROR(ERR_VAR_NOT_FOUND, { .real = lookup });
			}
			free(lookup);
			type = new_var->type;
		}
		else {
			free(name);
			return FMT_ERROR(ERR_INVALID_INPUT, { .tok = node->next->token });
		}
		var = make_variable(type, name, false);
	}
	else {
		char32_t *tmp_name = token_str(token->next);
		char *type_name = c32stombs(tmp_name);
		free(tmp_name);

		var = make_variable(find_type(type_name), name, false);
		free(type_name);
	}

	const char32_t *tmp = eval_assign(var, node->next, scope);
	var->is_const = is_const;

	if (tmp) {
		free_variable(var);
		free(name);
		return tmp;
	}
	if (scope_add_var(scope, var)) {
		free(name);
		return NULL;
	}
	free_variable(var);

	return FMT_ERROR(ERR_VAR_ALREADY_DEFINED, { .real = name });
}
