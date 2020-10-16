#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/eval/types/defs.h"
#include "skull/parse/classify.h"

#include "skull/llvm/aliases.h"

#include "skull/llvm/var.h"

#define PANIC(str) \
	char *const panic_str = c32stombs(str); \
	printf("Compilation error: %s\n", panic_str); \
	free(panic_str); \
	exit(1)

extern LLVMBuilderRef builder;

/*
Convert a Skull variable `var` into the LLVM IR equivalent.
*/
void var_to_llvm_ir(Variable *var) {
	char *const var_name = c32stombs(var->name);

	if (var->type == &TYPE_INT) {
		if (!var->alloca) {
			var->alloca = LLVMBuildAlloca(
				builder,
				LLVMInt64Type(),
				var_name
			);
		}

		SkullInt num = 0;
		variable_read(&num, var);

		LLVMBuildStore(
			builder,
			LLVM_INT(num),
			var->alloca
		);
	}
	if (var->type == &TYPE_FLOAT) {
		if (!var->alloca) {
			var->alloca = LLVMBuildAlloca(
				builder,
				LLVMDoubleType(),
				var_name
			);
		}

		SkullFloat num = 0;
		variable_read(&num, var);

		LLVMBuildStore(
			builder,
			LLVM_FLOAT(num),
			var->alloca
		);
	}
	else if (var->type == &TYPE_BOOL) {
		if (!var->alloca) {
			var->alloca = LLVMBuildAlloca(
				builder,
				LLVMInt1Type(),
				var_name
			);
		}

		bool val = false;
		variable_read(&val, var);

		LLVMBuildStore(
			builder,
			LLVM_BOOL(val),
			var->alloca
		);
	}
	else if (var->type == &TYPE_RUNE) {
		if (!var->alloca) {
			var->alloca = LLVMBuildAlloca(
				builder,
				LLVMInt32Type(),
				var_name
			);
		}

		char32_t c = U'\0';
		variable_read(&c, var);

		LLVMBuildStore(
			builder,
			LLVM_RUNE(c),
			var->alloca
		);
	}
	else if (var->type == &TYPE_STR) {
		const char32_t *str = NULL;
		variable_read(&str, var);
		char *const mbs = c32stombs(str);
		const unsigned len = (unsigned)strlen(mbs);

		LLVMBuildStore(
			builder,
			LLVMConstString(mbs, len, false),
			LLVMBuildAlloca(
				builder,
				LLVMArrayType(
					LLVMInt8Type(),
					len + 1
				),
				""
			)
		);
		free(mbs);

		LLVMTypeRef str_ptr = LLVMPointerType(LLVMInt8Type(), 0);

		if (!var->alloca) {
			var->alloca = LLVMBuildAlloca(
				builder,
				str_ptr,
				var_name
			);
		}

		LLVMBuildStore(
			builder,
			LLVMBuildBitCast(
				builder,
				var->alloca,
				str_ptr,
				""
			),
			var->alloca
		);
	}

	free(var_name);
}

/*
Make and add a variable from `node` to `scope`.
*/
void node_make_var(const AstNode *const node, Scope *const scope) {
	const Token *token = node->token;
	const bool is_const = ATTR(AstNodeVarDef, node, is_const);
	if (!is_const) {
		token = token->next;
	}

	if (!node->next) {
		PANIC(FMT_ERROR(ERR_MISSING_ASSIGNMENT, { .tok = token }));
	}

	char32_t *const name = token_str(token);
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
		else if (node->next->node_type == AST_NODE_IDENTIFIER) {
			char32_t *const lookup = token_str(node->next->token);
			Variable *const new_var = scope_find_name(scope, lookup);

			if (!new_var) {
				free(name);
				PANIC(FMT_ERROR(ERR_VAR_NOT_FOUND, { .real = lookup }));
			}
			free(lookup);
			type = new_var->type;
		}
		else {
			free(name);
			PANIC(FMT_ERROR(ERR_INVALID_INPUT, { .tok = node->next->token }));
		}
		var = make_variable(type, name, is_const);
	}
	else {
		char32_t *const tmp_name = token_str(token->next);
		char *const type_name = c32stombs(tmp_name);
		free(tmp_name);

		var = make_variable(find_type(type_name), name, is_const);
		free(type_name);
	}

	if (scope_add_var(scope, var)) {
		free(name);
		return;
	}
	free_variable(var);

	PANIC(FMT_ERROR(ERR_VAR_ALREADY_DEFINED, { .real = name }));
}
