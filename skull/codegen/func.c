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
#include "skull/compiler/types/types.h"

#include "skull/codegen/func.h"

Expr gen_node(AstNode *, bool *);

static bool gen_function_def(const AstNode *const, FunctionDeclaration *);

static FunctionDeclaration *create_function(
	const AstNodeFunctionProto *const,
	char *,
	bool
);

static void state_add_func(FunctionDeclaration *, char *);

static LLVMTypeRef *parse_func_param(
	const AstNodeFunctionProto *const,
	FunctionDeclaration *const,
	bool *
);

/*
Parse declaration (and potential definition) of function in `node`.

Return `true` if an error occurred.
*/
bool gen_stmt_func_decl(const AstNode *const node) {
	const Token *const func_name_token = node->func_proto->name_tok;

	char *func_name = token_mbs_str(func_name_token);

	if (scope_find_name(SKULL_STATE.scope, func_name)) {
		FMT_ERROR(ERR_NO_REDEFINE_VAR_AS_FUNC, {
			.loc = &func_name_token->location,
			.real = func_name
		});

		return true;
	}

	FunctionDeclaration *found_func = find_func_by_name(func_name);

	if (found_func) {
		FMT_ERROR(ERR_NO_REDEFINE_FUNC, {
			.loc = &func_name_token->location,
			.real = func_name
		});

		return true;
	}

	const bool is_external = node->func_proto->is_external;
	const bool is_export = node->func_proto->is_export;

	FunctionDeclaration *func = create_function(
		node->func_proto,
		func_name,
		is_export || is_external
	);
	if (!func) {
		free(func_name);
		return true;
	}

	func->location = func_name_token->location;

	if (!is_external)
		return gen_function_def(node, func);

	return false;
}

/*
Add new LLVM function named `name` from `func_proto`.

If `is_private` is true the function will be private (statically linked).

Else, the function will be globally available.

Return `NULL` if an error occurred.
*/
static FunctionDeclaration *create_function(
	const AstNodeFunctionProto *const func_proto,
	char *name,
	bool is_private
) {
	FunctionDeclaration *func;
	func = Calloc(1, sizeof *func);

	char *return_type_name = func_proto->return_type_name;
	if (return_type_name)
		func->return_type = find_type(return_type_name);

	if (return_type_name && !func->return_type) {
		FMT_ERROR(ERR_TYPE_NOT_FOUND, { .str = return_type_name });

		free(func);
		return NULL;
	}

	bool err = false;
	LLVMTypeRef *params = parse_func_param(func_proto, func, &err);
	if (err) return NULL;

	func->type = LLVMFunctionType(
		gen_llvm_type(func->return_type),
		params,
		func->num_params,
		false
	);
	free(params);

	func->ref = LLVMAddFunction(
		SKULL_STATE.module,
		name,
		func->type
	);

	LLVMSetLinkage(
		func->ref,
		is_private ? LLVMExternalLinkage : LLVMPrivateLinkage
	);

	state_add_func(func, name);

	return func;
}

static void state_add_func(FunctionDeclaration *func, char *name) {
	if (!SKULL_STATE.function_decls) {
		SKULL_STATE.function_decls = ht_create();
	}
	func->name = name;
	ht_add(SKULL_STATE.function_decls, func->name, func);
}

/*
Setup `func` params by parsing `func_proto`.

Set `err` if an error occurred.
*/
static LLVMTypeRef *parse_func_param(
	const AstNodeFunctionProto *const func_proto,
	FunctionDeclaration *const func,
	bool *err
) {
	char **param_type_names = func_proto->param_type_names;
	func->param_names = func_proto->param_names;

	unsigned short num_params = func_proto->num_params;
	func->num_params = num_params;

	if (!param_type_names) return NULL;

	LLVMTypeRef *params = Calloc(num_params, sizeof(LLVMTypeRef));
	func->param_types = Calloc(num_params, sizeof(Type));

	for RANGE(i, num_params) {
		func->param_types[i] = find_type(param_type_names[i]);

		if (!func->param_types[i]) {
			FMT_ERROR(ERR_TYPE_NOT_FOUND, { .str = param_type_names[i] });

			free(func->param_types);
			free(func);
			free(params);

			*err = true;
			return NULL;
		}

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
	const Token *func_name_token = expr->func_call->func_name_tok;

	char *const func_name = token_mbs_str(func_name_token);

	FunctionDeclaration *function = find_func_by_name(func_name);
	free(func_name);

	if (!function) {
		FMT_ERROR(ERR_MISSING_DECLARATION, {
			.tok = func_name_token
		});

		*err = true;
		return (Expr){0};
	}

	function->was_called = true;

	unsigned short num_params = function->num_params;

	if (num_params != expr->func_call->num_values) {
		FMT_ERROR(ERR_INVALID_NUM_PARAMS, {
			.loc = &func_name_token->location
		});

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
			Variable *const param_var = make_variable(
				func->param_types[i],
				func->param_names[i],
				true
			);

			if (!scope_add_var(&SKULL_STATE.scope, param_var)) {
				FMT_ERROR(ERR_SHADOW_VAR, { .var = param_var });

				variable_no_warnings(param_var);
				free_variable(param_var);

				return true;
			}

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

	Scope *scope_copy;
	make_sub_scope(&SKULL_STATE.scope, &scope_copy);

	bool err = false;
	const Expr returned = gen_node(node->child, &err);

	restore_sub_scope(&SKULL_STATE.scope, &scope_copy);
	SKULL_STATE.current_func = old_func;

	if (err) return true;

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
