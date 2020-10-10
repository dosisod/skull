#include <stddef.h>
#include <stdlib.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/eval/eval_assign.h"
#include "skull/eval/types/bool.h"
#include "skull/eval/types/defs.h"
#include "skull/eval/types/int.h"
#include "skull/parse/classify.h"

#include "skull/llvm/aliases.h"
#include "skull/llvm/var.h"

#include "skull/llvm/ast.h"

#define PANIC(str) \
	char *const panic_str = c32stombs(str); \
	printf("Compilation error: %s\n", panic_str); \
	free(panic_str); \
	exit(1)

#define PANIC_ON_ERR(str) \
	if (str) { \
		PANIC(str); \
	}

// module-level variables only accessible from functions in this file
static Scope *scope;
static LLVMValueRef func;
static LLVMModuleRef module;

extern LLVMBuilderRef builder;

/*
Convert skull code from `str_` into LLVM IR (using `func_` and `module_`).
*/
void str_to_llvm_ir(char *const str_, LLVMValueRef func_, LLVMModuleRef module_) {
	char32_t *const str = mbstoc32s(str_);
	DIE_IF_MALLOC_FAILS(str);

	const char32_t *error = NULL;
	AstNode *const node = make_ast_tree(str, &error);

	if (!node) {
		PANIC(error);
	}

	scope = make_scope();
	func = func_;
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

		else if (node->node_type == AST_NODE_VAR_DEF) {
			llvm_make_var_def(&node);
		}

		else if (node->node_type == AST_NODE_IF) {
			llvm_make_if(node);
		}

		else if (node->node_type == AST_NODE_ADD_CONSTS) {
			llvm_make_add(&node);
		}

		else if (
			node->node_type == AST_NODE_IDENTIFIER &&
			node->token->next &&
			*node->token->next->begin == '['
		) {
			llvm_make_function(node);
		}

		else if (node->node_type == AST_NODE_VAR_ASSIGN) {
			llvm_make_assign(&node);
		}

		else {
			PANIC(FMT_ERROR(ERR_UNEXPECTED_TOKEN, { .tok = node->token }));
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
		char32_t *const var_name = token_str(node->token->next);
		const Variable *const found_var = scope_find_name(scope, var_name);

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
				LLVMInt64Type(),
				found_var->alloca,
				""
			)
		);
	}
	else {
		const char32_t *error = NULL;
		SkullInt *const num = eval_integer(node->token->next, &error);
		PANIC_ON_ERR(error);

		LLVMBuildRet(
			builder,
			LLVM_INT(*num)
		);

		free(num);
	}
}

/*
Builds a variable from `node`.
*/
void llvm_make_var_def(AstNode **node) {
	static size_t vars_used_last = 0;

	PANIC_ON_ERR(node_make_var(*node, scope));

	*node = (*node)->next;

	var_to_llvm_ir(scope->vars[scope->vars_used - 1]);
	vars_used_last++;
}

/*
Builds an if block from `node`.
*/
void llvm_make_if(AstNode *node) {
	LLVMValueRef cond;

	if (node->token->next->token_type == TOKEN_BOOL_CONST) {
		const char32_t *error = NULL;
		bool *const tmp = eval_bool(node->token->next, &error);
		PANIC_ON_ERR(error);

		cond = LLVM_BOOL(*tmp);
		free(tmp);
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
			LLVMInt64Type(),
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

/*
Build LLVM for adding variables from `node`.
*/
void llvm_make_add(AstNode **node) {
	const Token *lhs = (*node)->token;
	const Token *rhs = (*node)->token->next->next;

	if (lhs->token_type == rhs->token_type) {
		*node = (*node)->next;
		return;
	}

	PANIC(FMT_ERROR(
		U"cannot add \"%\" and \"%\"",
		{ .tok = lhs },
		{ .tok = rhs }
	));
}

/*
Builds a function declaration from `node`.
*/
void llvm_make_function(AstNode *node) {
	LLVMTypeRef args[] = { LLVMVoidType() };

	LLVMTypeRef type = LLVMFunctionType(
		LLVMVoidType(),
		args,
		0,
		false
	);

	char32_t *const tmp = token_str(node->token);
	char *const func_name = c32stombs(tmp);
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

/*
Build a LLVM `load` operation from `node`.
*/
void llvm_make_assign(AstNode **node) {
	char32_t *const var_name = token_str((*node)->token);
	Variable *const found_var = scope_find_name(scope, var_name);

	if (!found_var) {
		PANIC(FMT_ERROR(ERR_VAR_NOT_FOUND, { .real = var_name }));
	}

	if (found_var->is_const) {
		PANIC(FMT_ERROR(ERR_CANNOT_ASSIGN_CONST, { .real = var_name }));
	}

	PANIC_ON_ERR(eval_assign(found_var, (*node)->next, scope));
	var_to_llvm_ir(found_var);

	free(var_name);

	*node = (*node)->next;
}