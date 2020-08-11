#include <stddef.h>
#include <stdlib.h>

#include "skull/eval/eval_integer.h"

#include "skullc/ast.h"

void str_to_llvm_ir(char32_t *str, LLVMBuilderRef builder, LLVMContextRef ctx) {
	ast_node_t *node = make_ast_tree(str);

	if (node->node_type == AST_NODE_RETURN) {
		const char32_t *error = NULL;
		int64_t *num = eval_integer(node->token->next, &error);

		LLVMBuildRet(
			builder,
			LLVMConstInt(
				LLVMInt64TypeInContext(ctx),
				(unsigned long long)*num,
				true
			)
		);
	}
	free(str);
}
