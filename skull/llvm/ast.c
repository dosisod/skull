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

// global variables only accessible from functions in this file
static Scope *scope;
static size_t vars_used_last;
static LLVMValueRef func;
static LLVMBuilderRef builder;
static LLVMContextRef ctx;
static LLVMModuleRef module;

/*
Convert skull code from `str` into LLVM IR (using `builder` and `ctx`).
*/
void str_to_llvm_ir(char *str_, LLVMValueRef func_, LLVMBuilderRef builder_, LLVMContextRef ctx_, LLVMModuleRef module_) {
	char32_t *str = mbstoc32s(str_);
	DIE_IF_MALLOC_FAILS(str);

	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(str, &error);

	if (!node) {
		PANIC(error);
	}

	scope = make_scope();
	vars_used_last = 0;
	func = func_;
	builder = builder_;
	ctx = ctx_;
	module = module_;

	node_to_llvm_ir(node);
	free(str);
}

/*
Internal LLVM IR parser.
*/
void node_to_llvm_ir(AstNode *node) {
	while (node) {
		if (node->node_type == AST_NODE_COMMENT) {}

		else if (node->node_type == AST_NODE_RETURN) {
			llvm_make_return(node);
		}

		else if (node->node_type == AST_NODE_VAR_DEF ||
			node->node_type == AST_NODE_AUTO_VAR_DEF ||
			node->node_type == AST_NODE_MUT_VAR_DEF ||
			node->node_type == AST_NODE_MUT_AUTO_VAR_DEF)
		{
			llvm_make_var_def(&node);
		}

		else if (node->node_type == AST_NODE_IF) {
			llvm_make_if(node);
		}

		else if (node->node_type == AST_NODE_IDENTIFIER && *node->token->next->begin == '[') {
			llvm_make_function(node);
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

/*
Builds an return statement from `node`.
*/
void llvm_make_return(AstNode *node) {
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

/*
Builds a variable from `node`.
*/
void llvm_make_var_def(AstNode **node) {
	PANIC_ON_ERR(repl_make_var(*node, scope,
		(*node)->node_type == AST_NODE_VAR_DEF ||
		(*node)->node_type == AST_NODE_AUTO_VAR_DEF
	));

	*node = (*node)->next;
}

/*
Builds an if block from `node`.
*/
void llvm_make_if(AstNode *node) {
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

	node_to_llvm_ir(node->child);

	LLVMBuildBr(builder, end);

	LLVMPositionBuilderAtEnd(
		builder,
		end
	);
}

/*
Builds a function declaration from `node`.
*/
void llvm_make_function(AstNode *node) {
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

	LLVMValueRef function = LLVMAddFunction(
		module,
		func_name,
		type
	);

	free(func_name);

	LLVMSetLinkage(function, LLVMExternalLinkage);

	LLVMBuildCall2(
		builder,
		type,
		function,
		NULL,
		0,
		""
	);
}