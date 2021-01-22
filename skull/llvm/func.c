#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/panic.h"
#include "skull/common/range.h"
#include "skull/common/str.h"
#include "skull/eval/types/types.h"
#include "skull/llvm/aliases.h"
#include "skull/llvm/scope.h"
#include "skull/llvm/var.h"

#include "skull/llvm/func.h"

extern LLVMBuilderRef BUILDER;
extern LLVMModuleRef MODULE;
extern LLVMValueRef CURRENT_FUNC;
extern LLVMValueRef MAIN_FUNC;
extern Scope *SCOPE;

bool node_to_llvm_ir(AstNode *);
FunctionDeclaration *llvm_create_new_function(
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
void declare_function(const AstNode *const node) {
	const bool is_external = ATTR(AstNodeFunctionProto, node, is_external);
	const bool is_export = ATTR(AstNodeFunctionProto, node, is_export);

	const Token *const func_name_token = (is_external || is_export) ?
			node->token->next :
			node->token;

	char *func_name = token_mbs_str(func_name_token);

	if (is_export && CURRENT_FUNC != MAIN_FUNC) {
		PANIC(ERR_NO_EXPORT_NESTED, {
			.tok = func_name_token
		});
	}

	if (strcmp(func_name, "main") == 0) {
		PANIC(ERR_MAIN_RESERVED, {0});
	}

	FunctionDeclaration *found_func = ht_get(FUNCTION_DECLARATIONS, func_name);

	if (found_func) {
		PANIC(ERR_NO_REDEFINE_FUNC, {
			.tok = func_name_token
		});
	}

	FunctionDeclaration *func = llvm_create_new_function(
		node,
		func_name,
		is_export || is_external
	);

	ht_add(FUNCTION_DECLARATIONS, func_name, func);

	if (!is_external) {
		define_function(node, func);
	}
}

/*
Create the actual LLVM function named `name` from `node`.

If `is_private` is true the function will be private (statically linked).

Else, the function will be globally available.
*/
FunctionDeclaration *llvm_create_new_function(
	const AstNode *const node,
	char *name,
	bool is_private
) {
	FunctionDeclaration *func;
	func = Calloc(1, sizeof *func);

	func->name = name;

	char **param_type_names = ATTR(
		AstNodeFunctionProto,
		node,
		param_type_names
	);
	func->param_names = ATTR(AstNodeFunctionProto, node, param_names);
	LLVMTypeRef *params = NULL;

	unsigned short num_params = ATTR(AstNodeFunctionProto, node, num_params);
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

			params[i] = func->param_types[i]->llvm_type();
		}
	}

	char *return_type_name = ATTR(AstNodeFunctionProto, node, return_type_name);
	if (return_type_name) {
		func->return_type = find_type(return_type_name);
	}

	if (return_type_name && !func->return_type) {
		PANIC(ERR_TYPE_NOT_FOUND, {
			.real = return_type_name
		});
	}

	LLVMTypeRef llvm_return_type = LLVMVoidType();

	if (func->return_type) {
		llvm_return_type = func->return_type->llvm_type();
	}

	func->type = LLVMFunctionType(
		llvm_return_type,
		params,
		num_params,
		false
	);
	free(params);

	func->function = LLVMAddFunction(
		MODULE,
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
Expr llvm_make_function_call(const AstNode *const node) {
	char32_t *const wide_func_name = token_str(node->token);
	char *const func_name = c32stombs(wide_func_name);

	FunctionDeclaration *function = ht_get(FUNCTION_DECLARATIONS, func_name);

	if (!function) {
		PANIC(ERR_MISSING_DECLARATION, {
			.tok = node->token
		});
	}
	free(func_name);
	free(wide_func_name);

	unsigned short num_params = function->num_params;

	LLVMValueRef *params = NULL;
	if (num_params) {
		params = Calloc(num_params, sizeof(LLVMValueRef));
	}

	const AstNode *param = node->child;

	if (num_params >= 1) {
		for RANGE(i, num_params) {
			const Expr expr = node_to_expr(
				function->param_types[i],
				param,
				NULL
			);

			if (expr.type != function->param_types[i]) {
				PANIC(ERR_FUNC_TYPE_MISMATCH, {
					.tok = param->token,
					.real = strdup(function->param_types[i]->name)
				});
			}

			if (param->token_end->next->type != TOKEN_COMMA &&
				i != (unsigned)(num_params - 1)
			) {
				PANIC(ERR_EXPECTED_COMMA, {
					.tok = param->token_end->next
				});
			}

			params[i] = expr.llvm_value;
			param = param->next;
		}
	}
	else if (param->token) {
		PANIC(ERR_ZERO_PARAM_FUNC, { .tok = param->token });
	}

	Expr ret = (Expr){
		.llvm_value = LLVMBuildCall2(
			BUILDER,
			function->type,
			function->function,
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
void define_function(const AstNode *const node, FunctionDeclaration *func) {
	LLVMBasicBlockRef current_block = LLVMGetLastBasicBlock(CURRENT_FUNC);

	LLVMValueRef old_func = CURRENT_FUNC;
	CURRENT_FUNC = func->function;

	LLVMBasicBlockRef entry = LLVMAppendBasicBlock(
		func->function,
		"entry"
	);

	LLVMPositionBuilderAtEnd(BUILDER, entry);

	MAKE_SUB_SCOPE;

	if (func->param_types) {
		LLVMValueRef next_param = LLVMGetFirstParam(func->function);

		for RANGE(i, func->num_params) {
			Variable *const param_var = make_variable(
				func->param_types[i],
				func->param_names[i],
				true
			);

			if (!scope_add_var(SCOPE, param_var)) {
				PANIC(ERR_SHADOW_VAR, { .var = param_var });
			}

			param_var->llvm_value = next_param;
			next_param = LLVMGetNextParam(next_param);
		}
	}

	bool returned = node_to_llvm_ir(node->child);

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

	if (!func->return_type) {
		LLVMBuildRetVoid(BUILDER);
	}

	LLVMPositionBuilderAtEnd(BUILDER, current_block);

	CURRENT_FUNC = old_func;
}

void free_function_declaration(FunctionDeclaration *func) {
	free(func->name);
	free(func->param_types);
	free(func);
}
