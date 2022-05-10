#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <llvm-c/Core.h>
#include <llvm-c/DebugInfo.h>

#include "skull/build_data.h"
#include "skull/codegen/llvm/aliases.h"
#include "skull/codegen/llvm/core.h"
#include "skull/codegen/llvm/debug.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/codegen/llvm/types.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/parse/ast_node.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/symbol.h"
#include "skull/semantic/types.h"

#include "skull/codegen/llvm/func.h"

static void gen_function_def(const AstNode *const, Symbol *, SkullStateLLVM *);
static void add_func(Symbol *, const SkullStateLLVM *);
static LLVMTypeRef *parse_func_param(
	const FunctionDeclaration *,
	const SkullStateLLVM *state
);

/*
Parse declaration (and potential definition) of function in `node`.
*/
void gen_stmt_func_decl(const AstNode *const node, SkullStateLLVM *state) {
	Symbol *symbol = node->func_proto->symbol;
	assert(symbol->type == SYMBOL_FUNC);

	add_func(symbol, state);

	if (!symbol->func->is_external) {
		gen_function_def(node, symbol, state);
	}
}

/*
Add new LLVM function from `symbol`.
*/
static void add_func(Symbol *symbol, const SkullStateLLVM *state) {
	FunctionDeclaration *func = symbol->func;

	LLVMTypeRef *params = parse_func_param(func, state);

	func->type = LLVMFunctionType(
		type_to_llvm_type(func->return_type, state),
		params,
		func->num_params,
		false
	);
	free(params);

	func->ref = LLVMAddFunction(
		state->module,
		symbol->linkage_name,
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
static LLVMTypeRef *parse_func_param(
	const FunctionDeclaration *func,
	const SkullStateLLVM *state
) {
	const unsigned num_params = func->num_params;

	LLVMTypeRef *params = Calloc(num_params, sizeof(LLVMTypeRef));

	for (unsigned i = 0; i < num_params; i++) {
		params[i] = type_to_llvm_type(func->param_types[i], state);
	}

	return params;
}

/*
Builds a function call from `func_call`.
*/
Expr gen_expr_func_call(
	const AstNodeFunctionCall *const func_call,
	const SkullStateLLVM *state
) {
	FunctionDeclaration *function = func_call->symbol->func;

	unsigned short num_params = function->num_params;

	LLVMValueRef *params = NULL;
	if (num_params)
		params = Calloc(num_params, sizeof(LLVMValueRef));

	const AstNode *param = func_call->params;

	for (unsigned i = 0; i < num_params; i++) {
		params[i] = gen_expr(param->expr, state).value;
		param = param->next;
	}

	const Expr ret = (Expr){
		.value = LLVMBuildCall2(
			state->builder,
			function->type,
			function->ref,
			params,
			num_params,
			""
		),
		.type = function->return_type
	};
	add_llvm_debug_info(ret.value, &func_call->func_name_tok->location, state);

	free(params);

	return ret;
}

/*
Create a native LLVM function.
*/
static void gen_function_def(
	const AstNode *const node,
	Symbol *symbol,
	SkullStateLLVM *state
) {
	FunctionDeclaration *func = symbol->func;

	if (func->param_types) {
		LLVMValueRef next_param = LLVMGetFirstParam(func->ref);

		for (unsigned i = 0; i < func->num_params; i++) {
			Variable *param_var = func->params[i]->symbol->var;

			param_var->ref = next_param;
			next_param = LLVMGetNextParam(next_param);
		}
	}

	LLVMBasicBlockRef current_block = LLVMGetLastBasicBlock(
		state->current_func->func->ref
	);

	LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(
		state->ctx,
		func->ref,
		"entry"
	);

	LLVMPositionBuilderAtEnd(state->builder, entry);

	Symbol *old_symbol = state->current_func;
	state->current_func = symbol;
	state->semantic->scope = state->semantic->scope->child;

	LLVMMetadataRef old_di_scope = add_llvm_func_debug_info(symbol, state);

	const Expr returned = gen_tree(node->child, state);

	restore_parent_scope(state->semantic);
	state->current_func = old_symbol;

	if (BUILD_DATA.debug) DEBUG_INFO.scope = old_di_scope;

	if (state->semantic->scope)
		state->semantic->scope = state->semantic->scope->next;

	if (func->return_type == &TYPE_VOID && returned.type != &TYPE_VOID)
		LLVMBuildRetVoid(state->builder);

	LLVMPositionBuilderAtEnd(state->builder, current_block);
}
