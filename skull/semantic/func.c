#include <stdbool.h>
#include <string.h>

#include "skull/codegen/scope.h"
#include "skull/codegen/shared.h"
#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/range.h"
#include "skull/compiler/variable.h"

#include "skull/semantic/func.h"

static void state_add_func(FunctionDeclaration *);


bool validate_stmt_func_decl(AstNode *node) {
	const bool is_external = node->func_proto->is_external;
	const bool is_export = node->func_proto->is_export;
	const Token *const func_name_token = node->func_proto->name_tok;

	if ((is_export || is_external) && !is_top_lvl_scope()) {
		FMT_ERROR(ERR_NO_NESTED, { .tok = func_name_token });
		return false;
	}

	char *func_name = token_mbs_str(func_name_token);

	if (is_export && strcmp(func_name, "main") == 0) {
		FMT_ERROR(ERR_MAIN_RESERVED, { .loc = &func_name_token->location });

		free(func_name);
		return false;
	}

	if (scope_find_name(SKULL_STATE.scope, func_name)) {
		FMT_ERROR(ERR_NO_REDEFINE_VAR_AS_FUNC, {
			.loc = &func_name_token->location,
			.real = func_name
		});

		return false;
	}

	FunctionDeclaration *found_func = find_func_by_name(func_name);

	if (found_func) {
		FMT_ERROR(ERR_NO_REDEFINE_FUNC, {
			.loc = &func_name_token->location,
			.real = func_name
		});

		return false;
	}

	Type return_type = NULL;
	char *return_type_name = node->func_proto->return_type_name;

	if (return_type_name)
		return_type = find_type(return_type_name);

	if (return_type_name && !return_type) {
		FMT_ERROR(ERR_TYPE_NOT_FOUND, { .str = return_type_name });

		free(func_name);
		return false;
	}

	FunctionDeclaration *func;
	func = Calloc(1, sizeof *func);
	func->name = func_name;
	func->location = func_name_token->location;
	func->is_external = is_external;
	func->is_export = is_export;
	func->return_type = return_type;
	func->param_names = node->func_proto->param_names;
	func->num_params = node->func_proto->num_params;

	node->func_proto->func = func;
	state_add_func(func);

	char **param_type_names = node->func_proto->param_type_names;
	if (!param_type_names) return true;

	func->param_types = Calloc(func->num_params, sizeof(Type));

	for RANGE(i, func->num_params) {
		func->param_types[i] = find_type(param_type_names[i]);

		if (!func->param_types[i]) {
			FMT_ERROR(ERR_TYPE_NOT_FOUND, { .str = param_type_names[i] });

			return false;
		}

		Variable *const param_var = make_variable(
			func->param_types[i],
			func->param_names[i],
			true
		);

		if (func->is_external) variable_no_warnings(param_var);

		if (!scope_add_var(&SKULL_STATE.scope, param_var)) {
			FMT_ERROR(ERR_SHADOW_VAR, { .var = param_var });

			variable_no_warnings(param_var);
			free_variable(param_var);

			return false;
		}
	}

	return true;
}

static void state_add_func(FunctionDeclaration *func) {
	if (!SKULL_STATE.function_decls) {
		SKULL_STATE.function_decls = ht_create();
	}
	ht_add(SKULL_STATE.function_decls, func->name, func);
}
