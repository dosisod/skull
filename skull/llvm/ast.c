#include <stddef.h>
#include <stdlib.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/eval/eval_integer.h"
#include "skull/eval/repl.h"
#include "skull/eval/types/bool.h"
#include "skull/eval/types/defs.h"
#include "skull/parse/classify.h"

#include "skull/llvm/aliases.h"
#include "skull/llvm/var.h"

#include "skull/llvm/ast.h"

#define PANIC(str) \
	printf("Compilation error: %s\n", c32stombs(str)); \
	exit(1)

#define PANIC_ON_ERR(str) \
	if (str) { \
		PANIC(str); \
	}

/*
Convert skull code from `str` into LLVM IR (using `builder`, `ctx`,  and `scope`).
*/
void str_to_llvm_ir(char *str_, LLVMValueRef func, LLVMBuilderRef builder, LLVMContextRef ctx) {
	char32_t *str = mbstoc32s(str_);
	DIE_IF_MALLOC_FAILS(str);

	AstNode *node = make_ast_tree(str);

	Scope *scope = make_scope();
	size_t vars_used_last = 0;

	while (node) {
		if (node->node_type == AST_NODE_COMMENT) {}

		else if (node->node_type == AST_NODE_RETURN) {
			llvm_make_return(&node, scope, ctx, builder);
		}

		else if (node->node_type == AST_NODE_VAR_DEF ||
			node->node_type == AST_NODE_AUTO_VAR_DEF ||
			node->node_type == AST_NODE_MUT_VAR_DEF ||
			node->node_type == AST_NODE_MUT_AUTO_VAR_DEF)
		{
			llvm_make_var_def(&node, scope);
		}

		else if (node->node_type == AST_NODE_IF) {
			llvm_make_if(&node, func, ctx, builder);
		}

		else {
			PANIC(FMT_ERROR(ERR_UNEXPECTED_TOKEN, { .tok = node->token }));
		}

		if (scope->vars_used != vars_used_last) {
			var_to_llvm_ir(scope->vars[scope->vars_used - 1], builder, ctx);
			vars_used_last++;
		}
		node = node->next;
	}
	free(str);
}

void llvm_make_return(AstNode **node, Scope *scope, LLVMContextRef ctx, LLVMBuilderRef builder) {
	if ((*node)->token->next->token_type == TOKEN_IDENTIFIER) {
		char32_t *var_name = token_str((*node)->token->next);
		const Variable *found_var = scope_find_name(scope, var_name);

		if (!found_var) {
			PANIC(FMT_ERROR(ERR_VAR_NOT_FOUND, { .real = var_name }));
		}
		free(var_name);

		if (found_var->type != &TYPE_INT) {
			PANIC(FMT_ERROR(ERR_NON_INT_RETURN, { .var = found_var }));
		}

		LLVMBuildRet(
			builder,
			LLVMBuildLoad2(
				builder,
				LLVMInt64TypeInContext(ctx),
				found_var->alloca,
				""
			)
		);
	}
	else {
		const char32_t *error = NULL;
		SkullInt *num = eval_integer((*node)->token->next, &error);
		PANIC_ON_ERR(error);

		LLVMBuildRet(
			builder,
			LLVM_INT(ctx, *num)
		);
	}
}

void llvm_make_var_def(AstNode **node, Scope *scope) {
	PANIC_ON_ERR(repl_make_var(*node, scope,
		(*node)->node_type == AST_NODE_VAR_DEF ||
		(*node)->node_type == AST_NODE_AUTO_VAR_DEF
	));

	*node = (*node)->next;
}

void llvm_make_if(AstNode **node, LLVMValueRef func, LLVMContextRef ctx, LLVMBuilderRef builder) {
	const char32_t *error = NULL;
	const bool *cond = eval_bool((*node)->token->next, &error);
	PANIC_ON_ERR(error);

	LLVMBasicBlockRef if_true = LLVMAppendBasicBlockInContext(
		ctx,
		func,
		"if_true"
	);

	LLVMBasicBlockRef end = LLVMAppendBasicBlockInContext(
		ctx,
		func,
		"end"
	);

	LLVMBuildCondBr(
		builder,
		LLVM_BOOL(ctx, *cond),
		if_true,
		end
	);

	if (!(*node)->child->token) {
		PANIC(FMT_ERROR(ERR_UNEXPECTED_TOKEN, { .tok = (*node)->token }));
	}
	Token *num_token = (*node)->child->token->next;

	SkullInt *num = eval_integer(num_token, &error);
	PANIC_ON_ERR(error);

	LLVMPositionBuilderAtEnd(
		builder,
		if_true
	);

	LLVMBuildRet(
		builder,
		LLVM_INT(ctx, *num)
	);

	LLVMPositionBuilderAtEnd(
		builder,
		end
	);
}