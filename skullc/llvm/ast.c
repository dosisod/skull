#include <stddef.h>
#include <stdlib.h>

#include "skull/eval/context.h"
#include "skull/eval/eval_integer.h"
#include "skull/eval/repl.h"

#include "skullc/llvm/aliases.h"
#include "skullc/llvm/var.h"

#include "skullc/llvm/ast.h"

/*
Convert skull code from `str` into LLVM IR (using `builder` and `ctx).
*/
void str_to_llvm_ir(char32_t *str, LLVMBuilderRef builder, LLVMContextRef llvm_ctx) {
	ast_node_t *node = make_ast_tree(str);

	context_t *ctx = make_context();
	size_t vars_used_last = 0;

while (node) {

	if (node->node_type == AST_NODE_RETURN) {
		const char32_t *error = NULL;
		int64_t *num = eval_integer(node->token->next, &error);

		LLVMBuildRet(
			builder,
			LLVM_INT(llvm_ctx, *num)
		);
	}
	else if (node->node_type == AST_NODE_VAR_DEF ||
		node->node_type == AST_NODE_AUTO_VAR_DEF)
	{
		repl_make_var(node, ctx, true);
		node = node->next;
	}

	if (ctx->vars_used != vars_used_last) {
		var_to_llvm_ir(ctx->vars[ctx->vars_used - 1], builder, llvm_ctx);
		vars_used_last++;
	}

node = node->next;
}

	free(str);
}
