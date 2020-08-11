#include <stddef.h>
#include <stdlib.h>

#include "skull/eval/context.h"
#include "skull/eval/eval_integer.h"
#include "skull/eval/repl.h"

#include "skullc/ast.h"
#include "skullc/llvm.h"
#include "skullc/var.h"

/*
Convert skull code from `str` into LLVM IR (using `builder` and `ctx).
*/
void str_to_llvm_ir(char32_t *str, LLVMBuilderRef builder, LLVMContextRef llvm_ctx) {
	ast_node_t *node = make_ast_tree(str);
	context_t *ctx = make_context();

	if (node->node_type == AST_NODE_RETURN) {
		const char32_t *error = NULL;
		int64_t *num = eval_integer(node->token->next, &error);

		LLVMBuildRet(
			builder,
			LLVM_INT(llvm_ctx, *num)
		);
	}
	else if (node->node_type==AST_NODE_VAR_DEF ||
		node->node_type==AST_NODE_AUTO_VAR_DEF)
	{
		repl_make_var(node, ctx, true);
	}
	free(str);

	if (ctx->vars_used) {
		var_to_llvm_ir(ctx->vars[0], builder, llvm_ctx);
	}
}
