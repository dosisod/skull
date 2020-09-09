#include <stddef.h>
#include <stdlib.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/eval/context.h"
#include "skull/eval/eval_integer.h"
#include "skull/eval/repl.h"
#include "skull/eval/types/bool.h"
#include "skull/eval/types/defs.h"
#include "skull/parse/classify.h"

#include "skullc/llvm/aliases.h"
#include "skullc/llvm/var.h"

#include "skullc/llvm/ast.h"

#define PANIC(str) \
	printf("Compilation error: %s\n", c32stombs(str)); \
	exit(1)

#define PANIC_ON_ERR(str) \
	if (str) { \
		PANIC(str); \
	}

/*
Convert skull code from `str` into LLVM IR (using `builder` and `ctx).
*/
void str_to_llvm_ir(char *str_, LLVMValueRef func, LLVMBuilderRef builder, LLVMContextRef llvm_ctx) {
	char32_t *str = mbstoc32s(str_);
	DIE_IF_MALLOC_FAILS(str);

	AstNode *node = make_ast_tree(str);

	Context *ctx = make_context();
	size_t vars_used_last = 0;

	while (node) {
		if (node->node_type == AST_NODE_COMMENT) {}

		else if (node->node_type == AST_NODE_RETURN) {
			llvm_make_return(&node, ctx, llvm_ctx, builder);
		}

		else if (node->node_type == AST_NODE_VAR_DEF ||
			node->node_type == AST_NODE_AUTO_VAR_DEF ||
			node->node_type == AST_NODE_MUT_VAR_DEF ||
			node->node_type == AST_NODE_MUT_AUTO_VAR_DEF)
		{
			llvm_make_var_def(&node, ctx);
		}

		else if (node->node_type == AST_NODE_IF) {
			llvm_make_if(&node, func, llvm_ctx, builder);
		}

		else {
			PANIC(FMT_ERROR(ERR_UNEXPECTED_TOKEN, { .tok = node->token }));
		}

		if (ctx->vars_used != vars_used_last) {
			var_to_llvm_ir(ctx->vars[ctx->vars_used - 1], builder, llvm_ctx);
			vars_used_last++;
		}
		node = node->next;
	}
	free(str);
}

void llvm_make_return(AstNode **node, Context *ctx, LLVMContextRef llvm_ctx, LLVMBuilderRef builder) {
	if ((*node)->token->next->token_type == TOKEN_IDENTIFIER) {
		char32_t *var_name = token_str((*node)->token->next);
		const Variable *found_var = context_find_name(ctx, var_name);

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
				LLVMInt64TypeInContext(llvm_ctx),
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
			LLVM_INT(llvm_ctx, *num)
		);
	}
}

void llvm_make_var_def(AstNode **node, Context *ctx) {
	PANIC_ON_ERR(repl_make_var(*node, ctx,
		(*node)->node_type == AST_NODE_VAR_DEF ||
		(*node)->node_type == AST_NODE_AUTO_VAR_DEF
	));

	*node = (*node)->next;
}

void llvm_make_if(AstNode **node, LLVMValueRef func, LLVMContextRef llvm_ctx, LLVMBuilderRef builder) {
	const char32_t *error = NULL;
	const bool *cond = eval_bool((*node)->token->next, &error);
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

	Token *num_token = (*node)->token->next->next->next->next;
	if ((*node)->token->next->next->next->token_type == TOKEN_NEWLINE) {
		num_token = num_token->next;
	}

	SkullInt *num = eval_integer(num_token, &error);
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