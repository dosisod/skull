#include <stdbool.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/codegen/aliases.h"
#include "skull/codegen/func.h"
#include "skull/codegen/oper.h"
#include "skull/codegen/scope.h"
#include "skull/codegen/shared.h"
#include "skull/codegen/types.h"
#include "skull/common/errors.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/compiler/scope.h"
#include "skull/compiler/types/defs.h"

#include "skull/codegen/assign.h"

void assign_value_to_var(LLVMValueRef, Variable *const);

/*
Builds a variable from `node`.
*/
void gen_stmt_var_def(AstNode **node) {
	Variable *var = node_to_var(*node);

	assign_value_to_var(
		node_to_expr(var->type, (*node)->next, var).llvm_value,
		var
	);

	*node = (*node)->next;
}

/*
Build a LLVM `load` operation from `node`.
*/
void gen_stmt_var_assign(AstNode **node) {
	Variable *found_var = scope_find_var((*node)->token);

	if (found_var->is_const) {
		PANIC(ERR_REASSIGN_CONST, {
			.tok = (*node)->token
		});
	}

	assign_value_to_var(
		node_to_expr(found_var->type, (*node)->next, found_var).llvm_value,
		found_var
	);

	*node = (*node)->next;
}

/*
Create an expression from `node` with type `type`.

Optionally pass `var` if expression is going to be assigned to a variable.
*/
Expr node_to_expr(
	const Type *const type,
	const AstNode *const node,
	const Variable *const var
) {
	Expr expr = {0};

	if (node->type == AST_NODE_EXPR) {
		expr = gen_expr_oper(type, node->attr.expr, var);
	}

	if (!expr.llvm_value) {
		PANIC(ERR_INVALID_EXPR, { .tok = node->token });
	}

	return expr;
}

/*
Assign `value` to `var`.
*/
void assign_value_to_var(LLVMValueRef value, Variable *const var) {
	const bool is_first_assign = !var->llvm_value;
	const bool is_const_literal = LLVMIsConstant(value);

	const bool is_global = is_first_assign ?
		(SKULL_STATE.current_func == SKULL_STATE.main_func) :
		var->is_global;

	if (is_first_assign) {
		if (is_global && (!var->is_const || !is_const_literal)) {
			var->llvm_value = LLVMAddGlobal(
				SKULL_STATE.module,
				gen_llvm_type(var->type),
				var->name
			);

			LLVMSetLinkage(var->llvm_value, LLVMPrivateLinkage);

			LLVMSetInitializer(
				var->llvm_value,
				LLVMConstNull(gen_llvm_type(var->type))
			);
		}
		else if (!is_global && !var->is_const) {
			var->llvm_value = LLVMBuildAlloca(
				SKULL_STATE.builder,
				gen_llvm_type(var->type),
				var->name
			);
		}

		var->is_global = is_global;
		var->is_const_lit = is_const_literal;
	}

	if (var->is_const && !(is_global && !is_const_literal)) {
		var->llvm_value = value;
	}
	else {
		LLVMBuildStore(
			SKULL_STATE.builder,
			value,
			var->llvm_value
		);
	}
}

/*
Create a type alias from `node`.
*/
void create_type_alias(AstNode **node) {
	const Token *const token = (*node)->token;

	char *type_name = token_mbs_str(token->next->next);

	const bool added = add_alias(
		(Type *)find_type(type_name),
		token_mbs_str(token)
	);

	free(type_name);

	if (!added) {
		PANIC(ERR_ALIAS_ALREADY_DEFINED, { .tok = token });
	}
}
