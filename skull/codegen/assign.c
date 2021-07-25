#include <stdbool.h>
#include <string.h>

#include <llvm-c/Core.h>
#include <llvm-c/DebugInfo.h>

#include "skull/build_data.h"
#include "skull/codegen/expr.h"
#include "skull/codegen/llvm/debug.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/codegen/llvm/types.h"
#include "skull/common/errors.h"
#include "skull/parse/ast_node.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"

#include "skull/codegen/assign.h"

static void assign_value_to_var(LLVMValueRef, Variable *const);
static void setup_var_llvm(LLVMValueRef, Variable *);
static void add_llvm_var_def_debug_info(const Variable *);

/*
Builds a variable definition from `expr_node`.

Return `true` if an error occurred.
*/
bool gen_stmt_var_def(AstNodeVarDef *var_def) {
	Variable *var = var_def->var;

	bool err = false;
	LLVMValueRef value = node_to_expr(
		var->type,
		var_def->expr_node,
		&err
	).value;

	if (err) {
		variable_disable_warnings(var);
		return true;
	}

	setup_var_llvm(value, var);
	assign_value_to_var(value, var);
	add_llvm_var_def_debug_info(var);

	return false;
}

static void add_llvm_var_def_debug_info(const Variable *var) {
	if (!BUILD_DATA.debug) return;

	LLVMMetadataRef di_var = LLVMDIBuilderCreateAutoVariable(
		DEBUG_INFO.builder,
		DEBUG_INFO.scope,
		var->name, strlen(var->name),
		DEBUG_INFO.file,
		var->location.line,
		type_to_di_type(var->type),
		true,
		LLVMDIFlagZero,
		8
	);

	LLVMDIBuilderInsertDeclareAtEnd(
		DEBUG_INFO.builder,
		var->ref,
		di_var,
		LLVMDIBuilderCreateExpression(DEBUG_INFO.builder, NULL, 0),
		make_llvm_debug_location(&var->location),
		LLVMGetLastBasicBlock(SKULL_STATE_LLVM.current_func->ref)
	);
}

/*
Assign a to a variable from `var_assign`.

Return `true` if an error occurred.
*/
bool gen_stmt_var_assign(AstNodeVarAssign *var_assign) {
	Variable *var = var_assign->var;

	bool err = false;
	LLVMValueRef value = node_to_expr(
		var->type,
		var_assign->expr_node,
		&err
	).value;

	if (err) return true;

	assign_value_to_var(value, var);

	return false;
}

static void setup_var_llvm(LLVMValueRef value, Variable *var) {
	const bool is_const_literal = LLVMIsConstant(value);
	const bool is_export = var->is_exported;

	const bool is_global = \
		SKULL_STATE_LLVM.current_func == SKULL_STATE_LLVM.main_func;

	if (is_global && (!var->is_const || !is_const_literal || is_export)) {
		var->ref = LLVMAddGlobal(
			SKULL_STATE_LLVM.module,
			type_to_llvm_type(var->type),
			var->name
		);

		LLVMSetLinkage(
			var->ref,
			is_export ? LLVMExternalLinkage : LLVMPrivateLinkage
		);

		LLVMSetInitializer(
			var->ref,
			is_export ? value : LLVMConstNull(type_to_llvm_type(var->type))
		);
	}
	else if (!is_global && !var->is_const) {
		var->ref = LLVMBuildAlloca(
			SKULL_STATE_LLVM.builder,
			type_to_llvm_type(var->type),
			var->name
		);
	}
	else if (is_const_literal && !var->implicitly_typed) {
		FMT_WARN(WARN_TRIVIAL_TYPE, { .type = var->type });
	}

	var->is_global = is_global;
	var->is_const_lit = is_const_literal;
}

/*
Assign `value` to `var`.
*/
static void assign_value_to_var(LLVMValueRef value, Variable *const var) {
	const bool is_first_assign = !var->ref;
	const bool is_const_literal = LLVMIsConstant(value);

	const bool is_global = is_first_assign ?
		SKULL_STATE_LLVM.current_func == SKULL_STATE_LLVM.main_func :
		var->is_global;

	if (var->is_const && !(is_global && !is_const_literal)) {
		var->ref = value;
	}
	else {
		LLVMValueRef store = LLVMBuildStore(
			SKULL_STATE_LLVM.builder,
			value,
			var->ref
		);

		add_llvm_debug_info(store, &var->location);
	}
}
