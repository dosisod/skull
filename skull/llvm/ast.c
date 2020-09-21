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
void str_to_llvm_ir(char *str_, LLVMValueRef func, LLVMBuilderRef builder, LLVMContextRef ctx, LLVMModuleRef mod) {
	char32_t *str = mbstoc32s(str_);
	DIE_IF_MALLOC_FAILS(str);

	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(str, &error);

	if (!node) {
		PANIC(error);
	}

	str_to_llvm_ir_(node, func, builder, ctx, mod);
	free(str);
}

/*
Internal LLVM IR parser.
*/
void str_to_llvm_ir_(AstNode *node, LLVMValueRef func, LLVMBuilderRef builder, LLVMContextRef ctx, LLVMModuleRef mod) {
	Scope *scope = make_scope();
	size_t vars_used_last = 0;

	while (node) {
		if (node->node_type == AST_NODE_COMMENT) {}

		else if (node->node_type == AST_NODE_RETURN) {
			llvm_make_return(node, scope, ctx, builder);
		}

		else if (node->node_type == AST_NODE_VAR_DEF ||
			node->node_type == AST_NODE_AUTO_VAR_DEF ||
			node->node_type == AST_NODE_MUT_VAR_DEF ||
			node->node_type == AST_NODE_MUT_AUTO_VAR_DEF)
		{
			llvm_make_var_def(&node, scope);
		}

		else if (node->node_type == AST_NODE_IF) {
			llvm_make_if(node, scope, func, ctx, builder);
		}

		else if (node->node_type == AST_NODE_IDENTIFIER && *node->token->next->begin == '[') {
			llvm_make_func(node, ctx, builder, mod);
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
}

void llvm_make_return(AstNode *node, Scope *scope, LLVMContextRef ctx, LLVMBuilderRef builder) {
	if (node->node_type != AST_NODE_RETURN) {
		PANIC(U"Return expected");
	}

	if (node->token->next->token_type == TOKEN_IDENTIFIER) {
		char32_t *var_name = token_str(node->token->next);
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
		SkullInt *num = eval_integer(node->token->next, &error);
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

void llvm_make_if(AstNode *node, Scope *scope, LLVMValueRef func, LLVMContextRef ctx, LLVMBuilderRef builder) {
	const char32_t *error = NULL;
	const bool *cond = eval_bool(node->token->next, &error);
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

	if (!node->child->token) {
		PANIC(FMT_ERROR(ERR_UNEXPECTED_TOKEN, { .tok = node->token }));
	}
	LLVMPositionBuilderAtEnd(
		builder,
		if_true
	);

	llvm_make_return(node->child, scope, ctx, builder);

	LLVMPositionBuilderAtEnd(
		builder,
		end
	);
}

void llvm_make_func(AstNode *node, LLVMContextRef ctx, LLVMBuilderRef builder, LLVMModuleRef mod) {
	LLVMTypeRef args[] = {
		LLVMVoidTypeInContext(ctx)
	};

	LLVMTypeRef type = LLVMFunctionType(
		LLVMVoidTypeInContext(ctx),
		args,
		0,
		false
	);

	char32_t *tmp = token_str(node->token);
	char *func_name = c32stombs(tmp);
	free(tmp);

	LLVMValueRef func = LLVMAddFunction(
		mod,
		func_name,
		type
	);

	free(func_name);

	LLVMSetLinkage(func, LLVMExternalLinkage);

	LLVMBuildCall2(
		builder,
		type,
		func,
		NULL,
		0,
		""
	);
}