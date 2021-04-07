#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "skull/codegen/aliases.h"
#include "skull/codegen/scope.h"
#include "skull/codegen/shared.h"
#include "skull/codegen/types.h"
#include "skull/codegen/var.h"
#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/panic.h"
#include "skull/common/range.h"
#include "skull/common/str.h"
#include "skull/compiler/types/types.h"

#include "skull/codegen/func.h"

bool gen_node(AstNode *);

void define_function(const AstNode *const, FunctionDeclaration *);

FunctionDeclaration *add_function(
	const AstNode *const,
	char *,
	bool
);

Expr node_to_expr(
	const Type *const type,
	const AstNode *const,
	const Variable *const
);

/*
Parse declaration (and potential definition) of function in `node`.
*/
void gen_stmt_func_decl(const AstNode *const node) {
	const bool is_external = node->attr.func_proto->is_external;
	const bool is_export = node->attr.func_proto->is_export;
	const Token *const func_name_token = node->attr.func_proto->name_tok;

	if ((is_export || is_external) && SKULL_STATE.scope->sub_scope) {
		PANIC(ERR_NO_NESTED, { .tok = func_name_token });
	}

	char *func_name = token_mbs_str(func_name_token);

	if (is_export && strcmp(func_name, "main") == 0) {
		PANIC(ERR_MAIN_RESERVED, { .tok = func_name_token });
	}

	FunctionDeclaration *found_func = ht_get(
		SKULL_STATE.function_decls,
		func_name
	);

	if (found_func) {
		PANIC(ERR_NO_REDEFINE_FUNC, {
			.tok = func_name_token
		});
	}

	FunctionDeclaration *func = add_function(
		node,
		func_name,
		is_export || is_external
	);

	ht_add(SKULL_STATE.function_decls, func_name, func);

	if (!is_external)
		define_function(node, func);
}

/*
Add new LLVM function named `name` from `node`.

If `is_private` is true the function will be private (statically linked).

Else, the function will be globally available.
*/
FunctionDeclaration *add_function(
	const AstNode *const node,
	char *name,
	bool is_private
) {
	FunctionDeclaration *func;
	func = Calloc(1, sizeof *func);

	func->name = name;

	char **param_type_names = node->attr.func_proto->param_type_names;
	func->param_names = node->attr.func_proto->param_names;
	LLVMTypeRef *params = NULL;

	unsigned short num_params = node->attr.func_proto->num_params;
	func->num_params = num_params;

	if (param_type_names) {
		params = Malloc(num_params * sizeof(LLVMValueRef));
		func->param_types = Calloc(num_params, sizeof(Type *));

		for RANGE(i, num_params) {
			func->param_types[i] = find_type(param_type_names[i]);

			if (!func->param_types[i]) {
				PANIC(ERR_TYPE_NOT_FOUND, {
					.real = param_type_names[i]
				});
			}

			params[i] = gen_llvm_type(func->param_types[i]);
		}
	}

	char *return_type_name = node->attr.func_proto->return_type_name;
	if (return_type_name)
		func->return_type = find_type(return_type_name);

	if (return_type_name && !func->return_type) {
		PANIC(ERR_TYPE_NOT_FOUND, {
			.real = return_type_name
		});
	}

	LLVMTypeRef llvm_return_type = LLVMVoidType();

	if (func->return_type) {
		llvm_return_type = gen_llvm_type(func->return_type);
	}

	func->type = LLVMFunctionType(
		llvm_return_type,
		params,
		num_params,
		false
	);
	free(params);

	func->function = LLVMAddFunction(
		SKULL_STATE.module,
		name,
		func->type
	);

	LLVMSetLinkage(
		func->function,
		is_private ?
			LLVMExternalLinkage :
			LLVMPrivateLinkage
	);

	return func;
}

/*
Builds a function call from `node`.
*/
Expr gen_expr_function_call(const AstNode *const node, const Type *const type) {
	const Token *func_name_token = node->attr.expr->func_call->func_name_tok;

	char *const func_name = token_mbs_str(func_name_token);

	FunctionDeclaration *function = ht_get(SKULL_STATE.function_decls, func_name);

	if (!function) {
		PANIC(ERR_MISSING_DECLARATION, {
			.tok = func_name_token
		});
	}
	free(func_name);

	unsigned short num_params = function->num_params;

	if (num_params != node->attr.expr->func_call->num_values) {
		PANIC(ERR_INVALID_NUM_PARAMS, { .tok = func_name_token });
	}

	LLVMValueRef *params = NULL;
	if (num_params)
		params = Calloc(num_params, sizeof(LLVMValueRef));

	const AstNode *param = node->child;

	if (num_params == 0 && param->token) {
		PANIC(ERR_ZERO_PARAM_FUNC, { .tok = param->token });
	}

	for RANGE(i, num_params) {
		const Expr expr = node_to_expr(
			function->param_types[i],
			param,
			NULL
		);

		if (expr.type != function->param_types[i]) {
			PANIC(ERR_FUNC_TYPE_MISMATCH,
				{ .tok = param->token, .type = function->param_types[i] },
				{ .type = expr.type }
			);
		}

		params[i] = expr.llvm_value;
		param = param->next;
	}

	Expr ret = (Expr){
		.llvm_value = LLVMBuildCall2(
			SKULL_STATE.builder,
			function->type,
			function->function,
			params,
			num_params,
			""
		),
		.type = function->return_type
	};

	free(params);

	if (type && ret.type != type) {
		PANIC(ERR_ASSIGN_BAD_TYPE,
			{ .tok = node->token, .type = ret.type },
			{ .type = type }
		);
	}

	return ret;
}

/*
Create a native LLVM function.
*/
void define_function(const AstNode *const node, FunctionDeclaration *func) {
	LLVMBasicBlockRef current_block = LLVMGetLastBasicBlock(
		SKULL_STATE.current_func->function
	);

	FunctionDeclaration *old_func = SKULL_STATE.current_func;
	SKULL_STATE.current_func = func;

	LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(
		SKULL_STATE.ctx,
		func->function,
		"entry"
	);

	LLVMPositionBuilderAtEnd(SKULL_STATE.builder, entry);

	MAKE_SUB_SCOPE;

	if (func->param_types) {
		LLVMValueRef next_param = LLVMGetFirstParam(func->function);

		for RANGE(i, func->num_params) {
			Variable *const param_var = make_variable(
				func->param_types[i],
				func->param_names[i],
				true
			);

			if (!scope_add_var(SKULL_STATE.scope, param_var)) {
				PANIC(ERR_SHADOW_VAR, { .var = param_var });
			}

			param_var->llvm_value = next_param;
			next_param = LLVMGetNextParam(next_param);
		}
	}

	bool returned = gen_node(node->child);

	RESTORE_SUB_SCOPE;

	if (!returned && func->return_type) {
		PANIC(ERR_EXPECTED_RETURN, {
			.real = func->name
		});
	}
	if (returned && !func->return_type) {
		PANIC(ERR_NO_VOID_RETURN, {
			.real = func->name
		});
	}

	if (!func->return_type)
		LLVMBuildRetVoid(SKULL_STATE.builder);

	LLVMPositionBuilderAtEnd(SKULL_STATE.builder, current_block);

	SKULL_STATE.current_func = old_func;
}

void free_function_declaration(HashItem *item) {
	FunctionDeclaration *func = item->data;

	if (!func) return;

	free(func->name);
	free(func->param_types);
	free(func);
}
