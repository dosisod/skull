#include <string.h>

#include <llvm-c/Core.h>

#include "skull/codegen/aliases.h"
#include "skull/codegen/func.h"
#include "skull/codegen/oper.h"
#include "skull/codegen/scope.h"
#include "skull/codegen/shared.h"
#include "skull/codegen/types.h"
#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/compiler/scope.h"

#include "skull/codegen/assign.h"

void assign_value_to_var(LLVMValueRef, Variable *const);
bool assert_sane_child(AstNode *);
bool _gen_stmt_var_assign(Variable *, AstNode **);

/*
Builds a variable definition from `node`.

Return `true` if an error occurred.
*/
bool gen_stmt_var_def(AstNode **node) {
	bool err = false;
	Variable *var = node_to_var(*node, &err);
	if (err) return true;

	return _gen_stmt_var_assign(var, node);
}

/*
Assign a to a variable from `node`.

Return `true` if an error occurred.
*/
bool gen_stmt_var_assign(AstNode **node) {
	bool err = false;
	Variable *var = scope_find_var((*node)->token, &err);
	if (err) return true;

	if (var->is_const) {
		FMT_ERROR(ERR_REASSIGN_CONST, {
			.tok = (*node)->token
		});

		return true;
	}
	var->was_reassigned = true;

	return _gen_stmt_var_assign(var, node);
}

/*
Does the actual assignment of `node` to `var`.

Return `true` if an error occurred.
*/
bool _gen_stmt_var_assign(Variable *var, AstNode **node) {
	bool err = false;

	LLVMValueRef value = node_to_expr(
		var->type,
		(*node)->next,
		&err
	).value;

	if (err) return true;

	assign_value_to_var(value, var);

	*node = (*node)->next;
	return !assert_sane_child(*node);
}

/*
Create an expression from `node` with type `type`.

Optionally pass `var` if expression is going to be assigned to a variable.

Set `err` if an error occurred.
*/
Expr node_to_expr(
	Type type,
	const AstNode *const node,
	bool *err
) {
	if (node && node->type == AST_NODE_EXPR) {
		const Expr expr = gen_expr_oper(type, node->expr, err);

		if (!expr.value && !*err) {
			FMT_ERROR(ERR_INVALID_EXPR, { .tok = node->token });
			*err = true;
		}

		return expr;
	}

	// node was not an expr, caller must handle this
	return (Expr){0};
}

/*
Assign `value` to `var`.
*/
void assign_value_to_var(LLVMValueRef value, Variable *const var) {
	const bool is_first_assign = !var->ref;
	const bool is_const_literal = LLVMIsConstant(value);

	const bool is_global = is_first_assign ?
		SKULL_STATE.current_func == &SKULL_STATE.main_func :
		var->is_global;

	if (is_first_assign) {
		if (is_global && (!var->is_const || !is_const_literal)) {
			var->ref = LLVMAddGlobal(
				SKULL_STATE.module,
				gen_llvm_type(var->type),
				var->name
			);

			LLVMSetLinkage(var->ref, LLVMPrivateLinkage);

			LLVMSetInitializer(
				var->ref,
				LLVMConstNull(gen_llvm_type(var->type))
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

/*
Create a type alias from `node`.

Return `true` if an error occurred.
*/
bool create_type_alias(AstNode **node) {
	const Token *const token = (*node)->token;

	char *type_name = token_mbs_str(token->next->next);

	char *alias = token_mbs_str(token);

	const bool added = add_alias(
		(Type)find_type(type_name),
		alias
	);

	free(type_name);

	if (added) return false;

	FMT_ERROR(ERR_ALIAS_ALREADY_DEFINED, {
		.loc = &token->location,
		.real = alias
	});

	return true;
}
