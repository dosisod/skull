#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <llvm-c/Core.h>
#include <llvm-c/DebugInfo.h>

#include "skull/build_data.h"
#include "skull/codegen/llvm/aliases.h"
#include "skull/codegen/llvm/debug.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/codegen/llvm/types.h"
#include "skull/common/malloc.h"
#include "skull/common/range.h"
#include "skull/common/str.h"
#include "skull/parse/ast_node.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/types.h"

#include "skull/codegen/llvm/func.h"

Expr gen_tree(const AstNode *);

static void gen_function_def(const AstNode *const, FunctionDeclaration *);
static void add_func(FunctionDeclaration *);
static LLVMTypeRef *parse_func_param(const FunctionDeclaration *);

/*
Parse declaration (and potential definition) of function in `node`.
*/
void gen_stmt_func_decl(const AstNode *const node) {
	FunctionDeclaration *func = node->func_proto->func;

	add_func(node->func_proto->func);

	if (!func->is_external) gen_function_def(node, func);
}

/*
Add new LLVM function from `func`.
*/
static void add_func(FunctionDeclaration *func) {
	LLVMTypeRef *params = parse_func_param(func);

	func->type = LLVMFunctionType(
		type_to_llvm_type(func->return_type),
		params,
		func->num_params,
		false
	);
	free(params);

	func->ref = LLVMAddFunction(
		SKULL_STATE_LLVM.module,
		func->name,
		func->type
	);

	LLVMSetLinkage(
		func->ref,
		(func->is_external || func->is_export) ?
			LLVMExternalLinkage :
			LLVMPrivateLinkage
	);
}

/*
Setup function params of `func`.
*/
static LLVMTypeRef *parse_func_param(const FunctionDeclaration *func) {
	const unsigned num_params = func->num_params;

	LLVMTypeRef *params = Calloc(num_params, sizeof(LLVMTypeRef));

	for RANGE(i, num_params) { // NOLINT
		params[i] = type_to_llvm_type(func->param_types[i]);
	}

	return params;
}

/*
Builds a function call from `func_call`.
*/
Expr gen_expr_func_call(const AstNodeFunctionCall *const func_call) {
	FunctionDeclaration *function = func_call->func_decl;

	unsigned short num_params = function->num_params;

	LLVMValueRef *params = NULL;
	if (num_params)
		params = Calloc(num_params, sizeof(LLVMValueRef));

	const AstNode *param = func_call->params;

	for RANGE(i, num_params) { // NOLINT
		params[i] = gen_expr(param->expr).value;
		param = param->next;
	}

	const Expr ret = (Expr){
		.value = LLVMBuildCall2(
			SKULL_STATE_LLVM.builder,
			function->type,
			function->ref,
			params,
			num_params,
			""
		),
		.type = function->return_type
	};
	add_llvm_debug_info(ret.value, &func_call->func_name_tok->location);

	free(params);

	return ret;
}

/*
Create a native LLVM function.
*/
static void gen_function_def(
	const AstNode *const node,
	FunctionDeclaration *func
) {
	if (func->param_types) {
		LLVMValueRef next_param = LLVMGetFirstParam(func->ref);

		for RANGE(i, func->num_params) {
			Variable *param_var = func->params[i]->var;

			param_var->ref = next_param;
			next_param = LLVMGetNextParam(next_param);
		}
	}

	LLVMBasicBlockRef current_block = LLVMGetLastBasicBlock(
		SKULL_STATE_LLVM.current_func->ref
	);

	LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(
		SKULL_STATE_LLVM.ctx,
		func->ref,
		"entry"
	);

	LLVMPositionBuilderAtEnd(SKULL_STATE_LLVM.builder, entry);

	FunctionDeclaration *old_func = SKULL_STATE_LLVM.current_func;
	SKULL_STATE_LLVM.current_func = func;
	SEMANTIC_STATE.scope = SEMANTIC_STATE.scope->child;

	LLVMMetadataRef old_di_scope = add_llvm_func_debug_info(func);

	const Expr returned = gen_tree(node->child);

	restore_parent_scope();
	SKULL_STATE_LLVM.current_func = old_func;

	if (BUILD_DATA.debug) DEBUG_INFO.scope = old_di_scope;

	if (SEMANTIC_STATE.scope)
		SEMANTIC_STATE.scope = SEMANTIC_STATE.scope->next;

	if (func->return_type == TYPE_VOID && returned.type != TYPE_VOID)
		LLVMBuildRetVoid(SKULL_STATE_LLVM.builder);

	LLVMPositionBuilderAtEnd(SKULL_STATE_LLVM.builder, current_block);
}
