#include <stdbool.h>

#include <llvm-c/Core.h>

#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/eval/scope.h"
#include "skull/eval/types/bool.h"
#include "skull/eval/types/int.h"
#include "skull/eval/variable.h"
#include "skull/llvm/aliases.h"
#include "skull/llvm/panic.h"
#include "skull/parse/classify.h"

#include "skull/llvm/flow.h"

extern Scope *scope;
extern LLVMValueRef func;
extern LLVMBuilderRef builder;

void node_to_llvm_ir(AstNode *);

/*
Builds an return statement from `node`.
*/
void llvm_make_return(AstNode *node) {
	if (node->node_type != AST_NODE_RETURN) {
		PANIC(U"Return expected");
	}

	if (node->token->next->token_type == TOKEN_IDENTIFIER) {
		char32_t *const var_name = token_str(node->token->next);
		const Variable *const found_var = scope_find_name(scope, var_name);

		if (!found_var) {
			PANIC(FMT_ERROR(ERR_VAR_NOT_FOUND, { .real = var_name }));
		}
		free(var_name);

		if (found_var->type != &TYPE_INT) {
			PANIC(FMT_ERROR(U"returning non-int variable \"%\" from main", { .var = found_var }));
		}

		LLVMBuildRet(
			builder,
			LLVMBuildLoad2(
				builder,
				LLVMInt64Type(),
				found_var->alloca,
				""
			)
		);
	}
	else {
		char32_t *error = NULL;

		LLVMBuildRet(
			builder,
			LLVM_INT(eval_integer(node->token->next, &error))
		);

		PANIC_ON_ERR(error);
	}
}

/*
Builds an if block from `node`.
*/
void llvm_make_if(AstNode *node) {
	LLVMValueRef cond;

	if (node->token->next->token_type == TOKEN_BOOL_CONST) {
		cond = LLVM_BOOL(eval_bool(node->token->next));
	}
	else {
		char32_t *const var_name = token_str(node->token->next);
		const Variable *const found_var = scope_find_name(scope, var_name);

		if (!found_var) {
			PANIC(FMT_ERROR(ERR_VAR_NOT_FOUND, { .real = var_name }));
		}
		free(var_name);

		if (found_var->type != &TYPE_BOOL) {
			PANIC(FMT_ERROR(U"Expected \"%\" to be of type bool", { .var = found_var }));
		}

		cond = LLVMBuildLoad2(
			builder,
			LLVMInt1Type(),
			found_var->alloca,
			""
		);
	}

	LLVMBasicBlockRef if_true = LLVMAppendBasicBlock(func, "if_true");
	LLVMBasicBlockRef end = LLVMAppendBasicBlock(func, "end");

	LLVMBuildCondBr(
		builder,
		cond,
		if_true,
		end
	);

	LLVMPositionBuilderAtEnd(
		builder,
		if_true
	);

	if (node->child->token) {
		node_to_llvm_ir(node->child);
	}

	LLVMBuildBr(builder, end);

	LLVMPositionBuilderAtEnd(
		builder,
		end
	);
}
