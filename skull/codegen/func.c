#include <stddef.h>
#include <string.h>

#include "skull/codegen/aliases.h"
#include "skull/codegen/scope.h"
#include "skull/codegen/shared.h"
#include "skull/codegen/types.h"
#include "skull/codegen/var.h"
#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/range.h"
#include "skull/common/str.h"
#include "skull/compiler/types/types.h"

#include "skull/codegen/func.h"

Expr gen_node(AstNode *, bool *);

bool define_function(const AstNode *const, FunctionDeclaration *);

FunctionDeclaration *add_function(
	const AstNode *const,
	char *,
	bool,
	bool *
);

Expr node_to_expr(
	Type type,
	const AstNode *const,
	const Variable *const,
	bool *
);

/*
Parse declaration (and potential definition) of function in `node`.

Return `true` if an error occurred.
*/
bool gen_stmt_func_decl(const AstNode *const node) {
	const bool is_external = node->func_proto->is_external;
	const bool is_export = node->func_proto->is_export;
	const Token *const func_name_token = node->func_proto->name_tok;

	if ((is_export || is_external) &&
		SKULL_STATE.scope &&
		SKULL_STATE.scope->sub_scope
	) {
		FMT_ERROR(ERR_NO_NESTED, { .tok = func_name_token });
		return true;
	}

	char *func_name = token_mbs_str(func_name_token);

	if (is_export && strcmp(func_name, "main") == 0) {
		FMT_ERROR(ERR_MAIN_RESERVED, { .loc = &func_name_token->location });

		free(func_name);
		return true;
	}

	FunctionDeclaration *found_func = ht_get(
		SKULL_STATE.function_decls,
		func_name
	);

	if (found_func) {
		FMT_ERROR(ERR_NO_REDEFINE_FUNC, {
			.loc = &func_name_token->location,
			.real = func_name
		});

		return true;
	}

	bool err = false;
	FunctionDeclaration *func = add_function(
		node,
		func_name,
		is_export || is_external,
		&err
	);
	if (err) return true;

	if (!SKULL_STATE.function_decls) {
		SKULL_STATE.function_decls = ht_create();
	}
	ht_add(SKULL_STATE.function_decls, func_name, func);

	if (!is_external)
		return define_function(node, func);

	return false;
}

/*
Add new LLVM function named `name` from `node`.

If `is_private` is true the function will be private (statically linked).

Else, the function will be globally available.

Set `err` if an error occurred.
*/
FunctionDeclaration *add_function(
	const AstNode *const node,
	char *name,
	bool is_private,
	bool *err
) {
	FunctionDeclaration *func;
	func = Calloc(1, sizeof *func);

	func->name = name;

	char **param_type_names = node->func_proto->param_type_names;
	func->param_names = node->func_proto->param_names;
	LLVMTypeRef *params = NULL;

	unsigned short num_params = node->func_proto->num_params;
	func->num_params = num_params;

	if (param_type_names) {
		params = Malloc(num_params * sizeof(LLVMValueRef));
		func->param_types = Calloc(num_params, sizeof(Type));

		for RANGE(i, num_params) {
			func->param_types[i] = find_type(param_type_names[i]);

			if (!func->param_types[i]) {
				FMT_ERROR(ERR_TYPE_NOT_FOUND, {
					.real = strdup(param_type_names[i])
				});

				// TODO(dosisod): merge with free_function_declaration
				free(func->name);
				free(func->param_types);
				free(func);
				free(params);

				*err = true;
				return NULL;
			}

			params[i] = gen_llvm_type(func->param_types[i]);
		}
	}

	char *return_type_name = node->func_proto->return_type_name;
	if (return_type_name)
		func->return_type = find_type(return_type_name);

	if (return_type_name && !func->return_type) {
		FMT_ERROR(ERR_TYPE_NOT_FOUND, {
			.real = strdup(return_type_name)
		});

		// TODO(dosisod): merge with free_function_declaration
		free(func->name);
		free(func->param_types);
		free(func);
		free(params);

		*err = true;
		return NULL;
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
Builds a function call from `expr`.

Set `err` if an error occurred.
*/
Expr gen_expr_function_call(
	const AstNodeExpr *const expr,
	Type type,
	bool *err
) {
	const Token *func_name_token = expr->func_call->func_name_tok;

	char *const func_name = token_mbs_str(func_name_token);

	FunctionDeclaration *function = ht_get(SKULL_STATE.function_decls, func_name);
	free(func_name);

	if (!function) {
		FMT_ERROR(ERR_MISSING_DECLARATION, {
			.tok = func_name_token
		});

		*err = true;
		return (Expr){0};
	}

	unsigned short num_params = function->num_params;

	if (num_params != expr->func_call->num_values) {
		FMT_ERROR(ERR_INVALID_NUM_PARAMS, { .loc = &func_name_token->location });

		*err = true;
		return (Expr){0};
	}

	LLVMValueRef *params = NULL;
	if (num_params)
		params = Calloc(num_params, sizeof(LLVMValueRef));

	const AstNode *param = expr->func_call->params;

	if (num_params == 0 && param->token) {
		FMT_ERROR(ERR_ZERO_PARAM_FUNC, { .loc = &param->token->location });

		*err = true;
		return (Expr){0};
	}

	for RANGE(i, num_params) {
		const Expr param_expr = node_to_expr(
			function->param_types[i],
			param,
			NULL,
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

	Expr ret = (Expr){
		.value = LLVMBuildCall2(
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
		FMT_ERROR(ERR_ASSIGN_BAD_TYPE,
			{ .loc = &func_name_token->location, .type = ret.type },
			{ .type = type }
		);

		*err = true;
		return (Expr){0};
	}

	return ret;
}

/*
Create a native LLVM function.
*/
bool define_function(const AstNode *const node, FunctionDeclaration *func) {
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

	Scope *scope_copy;
	make_sub_scope(&SKULL_STATE.scope, &scope_copy);

	if (func->param_types) {
		LLVMValueRef next_param = LLVMGetFirstParam(func->function);

		for RANGE(i, func->num_params) {
			Variable *const param_var = make_variable(
				func->param_types[i],
				func->param_names[i],
				true
			);

			if (!scope_add_var(&SKULL_STATE.scope, param_var)) {
				FMT_ERROR(ERR_SHADOW_VAR, { .var = param_var });

				// because we are "throwing" an error, this variable will not
				// be able to be read/wrote to, so we manually disable the
				// triggers so a warning is not displayed
				param_var->was_reassigned = true;
				param_var->was_read = true;
				free_variable(param_var);

				return true;
			}

			param_var->ref = next_param;
			next_param = LLVMGetNextParam(next_param);
		}
	}

	bool err = false;
	const Expr returned = gen_node(node->child, &err);
	restore_sub_scope(&SKULL_STATE.scope, &scope_copy);

	if (err) return true;

	if (!returned.value && func->return_type) {
		FMT_ERROR(ERR_EXPECTED_RETURN, {
			.real = func->name
		});

		return true;
	}
	if (returned.value && !func->return_type) {
		FMT_ERROR(ERR_NO_VOID_RETURN, {
			.real = func->name
		});

		return true;
	}

	if (!func->return_type)
		LLVMBuildRetVoid(SKULL_STATE.builder);

	LLVMPositionBuilderAtEnd(SKULL_STATE.builder, current_block);

	SKULL_STATE.current_func = old_func;

	return false;
}

void free_function_declaration(HashItem *item) {
	FunctionDeclaration *func = item->data;

	if (!func) return;

	free(func->name);
	free(func->param_types);
	free(func);
}
