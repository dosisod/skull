#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/codegen/llvm/aliases.h"
#include "skull/codegen/llvm/types.h"
#include "skull/codegen/scope.h"
#include "skull/codegen/shared.h"
#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/range.h"
#include "skull/common/str.h"
#include "skull/compiler/types.h"
#include "skull/parse/ast_node.h"

#include "skull/codegen/func.h"

Expr gen_node(AstNode *, bool *);

static bool gen_function_def(const AstNode *const, FunctionDeclaration *);

static void create_function(FunctionDeclaration *);

static LLVMTypeRef *parse_func_param(const FunctionDeclaration *);

/*
Parse declaration (and potential definition) of function in `node`.

Return `true` if an error occurred.
*/
bool gen_stmt_func_decl(const AstNode *const node) {
	FunctionDeclaration *func = node->func_proto->func;

	create_function(node->func_proto->func);

	if (!func->is_external) return gen_function_def(node, func);

	return false;
}

/*
Add new LLVM function from `func`.

Else, the function will be globally available.
*/
static void create_function(FunctionDeclaration *func) {
	LLVMTypeRef *params = parse_func_param(func);

	func->type = LLVMFunctionType(
		gen_llvm_type(func->return_type),
		params,
		func->num_params,
		false
	);
	free(params);

	func->ref = LLVMAddFunction(
		SKULL_STATE.module,
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
		params[i] = gen_llvm_type(func->param_types[i]);
	}

	return params;
}

/*
Builds a function call from `expr`.

Set `err` if an error occurred.
*/
Expr gen_expr_function_call(
	const AstNodeExpr *const expr,
	Type type,
	bool *err
) {
	const Token *func_name_token = expr->lhs.func_call->func_name_tok;
	char *const func_name = token_mbs_str(func_name_token);

	// TODO(dosisod): dont find func since we know it exists (store in node)
	FunctionDeclaration *function = find_func_by_name(func_name);
	free(func_name);

	unsigned short num_params = function->num_params;

	LLVMValueRef *params = NULL;
	if (num_params)
		params = Calloc(num_params, sizeof(LLVMValueRef));

	const AstNode *param = expr->lhs.func_call->params;

	for RANGE(i, num_params) {
		const Expr param_expr = node_to_expr(
			function->param_types[i],
			param,
			err
		);

		if (*err) {
			free(params);
			return (Expr){0};
		}

		if (param_expr.type != function->param_types[i]) {
			FMT_ERROR(ERR_FUNC_TYPE_MISMATCH,
				{
					.loc = &param->token->location,
					.type = function->param_types[i]
				},
				{ .type = param_expr.type }
			);

			free(params);
			*err = true;
			return (Expr){0};
		}

		params[i] = param_expr.value;
		param = param->next;
	}

	if (type && function->return_type != type) {
		FMT_ERROR(
			ERR_ASSIGN_BAD_TYPE,
			{
				.loc = &func_name_token->location,
				.type = function->return_type
			},
			{ .type = type }
		);

		*err = true;
		return (Expr){0};
	}

	const Expr ret = (Expr){
		.value = LLVMBuildCall2(
			SKULL_STATE.builder,
			function->type,
			function->ref,
			params,
			num_params,
			""
		),
		.type = function->return_type
	};

	free(params);

	return ret;
}

/*
Create a native LLVM function.
*/
static bool gen_function_def(
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
		SKULL_STATE.current_func->ref
	);

	LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(
		SKULL_STATE.ctx,
		func->ref,
		"entry"
	);

	LLVMPositionBuilderAtEnd(SKULL_STATE.builder, entry);

	FunctionDeclaration *old_func = SKULL_STATE.current_func;
	SKULL_STATE.current_func = func;

	SKULL_STATE.scope = SKULL_STATE.scope->child;

	bool err = false;
	const Expr returned = gen_node(node->child, &err);

	restore_parent_scope();
	SKULL_STATE.current_func = old_func;

	if (err) return true;
	if (SKULL_STATE.scope) SKULL_STATE.scope = SKULL_STATE.scope->next;

	if (!returned.value && func->return_type != TYPE_VOID) {
		FMT_ERROR(ERR_EXPECTED_RETURN, { .real = strdup(func->name) });

		return true;
	}
	if (returned.value && func->return_type == TYPE_VOID) {
		FMT_ERROR(ERR_NO_VOID_RETURN, { .real = strdup(func->name) });

		return true;
	}

	if (func->return_type == TYPE_VOID && returned.type != TYPE_VOID)
		LLVMBuildRetVoid(SKULL_STATE.builder);

	LLVMPositionBuilderAtEnd(SKULL_STATE.builder, current_block);

	return false;
}

/*
Return function declaration called `name`, or `NULL` if not found.
*/
FunctionDeclaration *find_func_by_name(const char *name) {
	return ht_get(SKULL_STATE.function_decls, name);
}

void free_function_declaration(HashItem *item) {
	FunctionDeclaration *func = item->data;

	if (!func) return;

	if (!func->was_called) {
		FMT_WARN(WARN_FUNC_UNUSED, {
			.real = func->name, .loc = &func->location
		});
	}
	else free(func->name);

	free(func->param_types);
	free(func);
}
