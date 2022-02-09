#include <stdbool.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/build_data.h"
#include "skull/codegen/llvm/debug.h"
#include "skull/codegen/llvm/expr.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/codegen/llvm/types.h"
#include "skull/parse/ast_node.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"

#include "skull/codegen/llvm/assign.h"

static void assign_value_to_var(
	LLVMValueRef,
	Variable *,
	const SkullStateLLVM *
);
static void setup_var_llvm(LLVMValueRef, Variable *, const SkullStateLLVM *);
static bool is_top_level(const SkullStateLLVM *);
static bool requires_alloca_decl(const Variable *);

/*
Builds a variable definition from `var_def`.
*/
void gen_stmt_var_def(const AstNodeVarDef *var_def, SkullStateLLVM *state) {
	Variable *var = var_def->var;

	// this is actually a type alias, nothing to generate
	if (!var) return;

	LLVMValueRef value = gen_expr(var_def->expr, state).value;

	setup_var_llvm(value, var, state);
	add_llvm_var_def_debug_info(var, state);
}

/*
Assign a to a variable from `var_assign`.
*/
void gen_stmt_var_assign(
	const AstNodeVarAssign *var_assign,
	SkullStateLLVM *state
) {
	LLVMValueRef value = gen_expr(var_assign->expr, state).value;

	assign_value_to_var(value, var_assign->var, state);
}

static void setup_var_llvm(
	LLVMValueRef value,
	Variable *var,
	const SkullStateLLVM *state
) {
	const bool is_const_literal = LLVMIsConstant(value);

	var->is_global = is_top_level(state);
	var->is_const_lit = is_const_literal;

	if (var->is_global) {
		var->ref = LLVMAddGlobal(
			state->module,
			type_to_llvm_type(var->type, state),
			var->name
		);

		LLVMSetLinkage(
			var->ref,
			var->is_exported ? LLVMExternalLinkage : LLVMPrivateLinkage
		);

		LLVMSetInitializer(
			var->ref,
			is_const_literal ?
				value :
				LLVMConstNull(type_to_llvm_type(var->type, state))
		);

		if (is_const_literal) return;
	}
	else if (requires_alloca_decl(var)) {
		var->ref = LLVMBuildAlloca(
			state->builder,
			type_to_llvm_type(var->type, state),
			var->name
		);
	}

	assign_value_to_var(value, var, state);
}

/*
Assign `value` to `var`.
*/
static void assign_value_to_var(
	LLVMValueRef value,
	Variable *var,
	const SkullStateLLVM *state
) {
	if (var->is_const && (!var->is_global || LLVMIsConstant(value))) {
		var->ref = value;
	}
	else {
		// value isnt constant, so we add a store instruction
		LLVMValueRef store = LLVMBuildStore(
			state->builder,
			value,
			var->ref
		);

		add_llvm_debug_info(store, &var->location, state);
	}
}

static bool is_top_level(const SkullStateLLVM *state) {
	return state->current_func == state->main_func;
}

static bool requires_alloca_decl(const Variable *var) {
	return !var->is_global && !var->is_const;
}
