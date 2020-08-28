#include <stddef.h>
#include <stdlib.h>

#include "skull/errors.h"
#include "skull/eval/context.h"
#include "skull/eval/eval_integer.h"
#include "skull/eval/repl.h"
#include "skull/eval/types/bool.h"

#include "skullc/llvm/aliases.h"
#include "skullc/llvm/var.h"

#include "skullc/llvm/ast.h"

#define PANIC_ON_ERR(ptr) \
	if (ptr) { \
		printf("Compilation error: %s\n", c32stombs(ptr)); \
		exit(1); \
	}

/*
Convert skull code from `str` into LLVM IR (using `builder` and `ctx).
*/
void str_to_llvm_ir(char32_t *str, LLVMValueRef func, LLVMBuilderRef builder, LLVMContextRef llvm_ctx) {
	ast_node_t *node = make_ast_tree(str);

	context_t *ctx = make_context();
	size_t vars_used_last = 0;

while (node) {

	if (node->node_type == AST_NODE_RETURN) {
		const char32_t *error = NULL;
		int64_t *num = eval_integer(node->token->next, &error);
		PANIC_ON_ERR(error);

		LLVMBuildRet(
			builder,
			LLVM_INT(llvm_ctx, *num)
		);
	}
	else if (node->node_type == AST_NODE_VAR_DEF ||
		node->node_type == AST_NODE_AUTO_VAR_DEF)
	{
		PANIC_ON_ERR(repl_make_var(node, ctx, true));

		node = node->next;
	}
	else if (node->node_type == AST_NODE_IF) {
		const char32_t *error = NULL;
		const bool *cond = eval_bool(node->token->next, &error);
		PANIC_ON_ERR(error);

		LLVMBasicBlockRef if_true = LLVMAppendBasicBlockInContext(
			llvm_ctx,
			func,
			"if_true"
		);

		LLVMBasicBlockRef end = LLVMAppendBasicBlockInContext(
			llvm_ctx,
			func,
			"end"
		);

		LLVMBuildCondBr(
			builder,
			LLVM_BOOL(llvm_ctx, *cond),
			if_true,
			end
		);

		int64_t *num = eval_integer(node->token->next->next->next->next, &error);
		PANIC_ON_ERR(error);

		LLVMPositionBuilderAtEnd(
			builder,
			if_true
		);

		LLVMBuildRet(
			builder,
			LLVM_INT(llvm_ctx, *num)
		);

		LLVMPositionBuilderAtEnd(
			builder,
			end
		);
	}
	else {
		PANIC_ON_ERR(ERR_UNEXPECTED_TOKEN);
	}

	if (ctx->vars_used != vars_used_last) {
		var_to_llvm_ir(ctx->vars[ctx->vars_used - 1], builder, llvm_ctx);
		vars_used_last++;
	}

node = node->next;
}

	free(str);
}
