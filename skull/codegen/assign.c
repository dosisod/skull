#include <stdbool.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/codegen/func.h"
#include "skull/codegen/llvm/types.h"
#include "skull/codegen/scope.h"
#include "skull/codegen/shared.h"
#include "skull/common/errors.h"

#include "skull/codegen/assign.h"

static void assign_value_to_var(LLVMValueRef, Variable *const);

/*
Builds a variable definition from `node`.

Return `true` if an error occurred.
*/
bool gen_stmt_var_def(AstNode *node) {
	Variable *var = scope_find_var(node->var_def->name_tok, true);

	bool err = false;
	LLVMValueRef value = node_to_expr(
		var->type,
		node->var_def->expr_node,
		&err
	).value;

	if (err) {
		variable_no_warnings(var);
		return true;
	}

	assign_value_to_var(value, var);

	return false;
}

/*
Assign a to a variable from `node`.

Return `true` if an error occurred.
*/
bool gen_stmt_var_assign(AstNode *node) {
	Variable *var = scope_find_var(node->token, false);

	bool err = false;
	LLVMValueRef value = node_to_expr(
		var->type,
		node->expr_node,
		&err
	).value;

	if (err) return true;

	assign_value_to_var(value, var);

	return false;
}

/*
Assign `value` to `var`.
*/
static void assign_value_to_var(LLVMValueRef value, Variable *const var) {
	const bool is_first_assign = !var->ref;
	const bool is_const_literal = LLVMIsConstant(value);
	const bool is_export = var->is_exported;

	const bool is_global = is_first_assign ?
		SKULL_STATE.current_func == SKULL_STATE.main_func :
		var->is_global;

	if (is_first_assign) {
		if (is_global && (!var->is_const || !is_const_literal || is_export)) {
			var->ref = LLVMAddGlobal(
				SKULL_STATE.module,
				gen_llvm_type(var->type),
				var->name
			);

			LLVMSetLinkage(
				var->ref,
				is_export ? LLVMExternalLinkage : LLVMPrivateLinkage
			);

			LLVMSetInitializer(
				var->ref,
				is_export ? value : LLVMConstNull(gen_llvm_type(var->type))
			);
		}
		else if (!is_global && !var->is_const) {
			var->ref = LLVMBuildAlloca(
				SKULL_STATE.builder,
				gen_llvm_type(var->type),
				var->name
			);
		}
		else if (is_const_literal && !var->implicitly_typed) {
			FMT_WARN(WARN_TRIVIAL_TYPE, { .type = var->type });
		}

		var->is_global = is_global;
		var->is_const_lit = is_const_literal;
	}

	if (var->is_const && !(is_global && !is_const_literal)) {
		var->ref = value;
	}
	else {
		LLVMBuildStore(
			SKULL_STATE.builder,
			value,
			var->ref
		);
	}
}
