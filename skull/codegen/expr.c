#include <stdbool.h>

#include <llvm-c/Core.h>

#include "skull/codegen/func.h"
#include "skull/codegen/llvm/aliases.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/codegen/llvm/types.h"
#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/parse/ast_node.h"
#include "skull/semantic/scope.h"

#include "skull/codegen/expr.h"

typedef Expr (Operation)(const Expr *, LLVMValueRef);

typedef LLVMValueRef (LLVMBuildX)(
	LLVMBuilderRef,
	LLVMValueRef,
	LLVMValueRef,
	const char *
);

static Operation gen_expr_add, gen_expr_sub, gen_expr_mult,
	gen_expr_not, gen_expr_unary_neg, gen_expr_is, gen_expr_is_not,
	gen_expr_less_than, gen_expr_gtr_than, gen_expr_less_than_eq,
	gen_expr_gtr_than_eq, gen_expr_lshift, gen_expr_rshift,
	gen_expr_and, gen_expr_or, gen_expr_xor, gen_expr_div, gen_expr_mod;

static Expr gen_expr_pow(
	const Expr *,
	LLVMValueRef,
	bool *
);

static Expr gen_expr_const(const AstNodeExpr *);
static Expr gen_expr_is_str(LLVMValueRef, LLVMValueRef);
static Expr gen_expr(Type, AstNodeExpr *const, bool *);
static Expr ident_to_expr(AstNodeExpr *, Variable **);
static Expr gen_expr_identifier(Type, AstNodeExpr *, bool *);
static Operation *expr_type_to_func(ExprType);
static bool is_bool_expr(ExprType);

static Expr create_and_call_builtin_oper(
	Type,
	LLVMTypeRef,
	const char *,
	LLVMValueRef,
	LLVMValueRef
);

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
		const Expr expr = gen_expr(type, node->expr, err);

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
Return expression for `expr`, checking if resulting type matches `type`.

Set `err` if an error occurred.
*/
static Expr gen_expr(
	Type type,
	AstNodeExpr *const expr,
	bool *err
) {
	switch (expr->oper) {
		case EXPR_IDENTIFIER:
			return gen_expr_identifier(type, expr, err);
		case EXPR_CONST:
			return gen_expr_const(expr);
		case EXPR_FUNC:
			return gen_expr_func_call(expr->lhs.func_call, type, err);
		default: break;
	}

	const Expr lhs = expr->lhs.expr ?
		gen_expr(is_bool_expr(expr->oper) ? NULL : type, expr->lhs.expr, err) :
		(Expr){0};

	if (*err) return (Expr){0};

	const Expr rhs = gen_expr(
		is_bool_expr(expr->oper) ? lhs.type : type,
		expr->rhs,
		err
	);
	if (*err) return (Expr){0};

	const Token *rhs_token = expr->rhs->lhs.tok;

	if (lhs.value && lhs.type != rhs.type) {
		FMT_ERROR(ERR_EXPECTED_SAME_TYPE,
			{ .loc = &rhs_token->location, .type = lhs.type },
			{ .type = rhs.type }
		);

		*err = true;
		return (Expr){0};
	}

	Expr result = (Expr){0};

	Operation *func = expr_type_to_func(expr->oper);
	if (func) {
		result = func(
			&(Expr){ .type = rhs.type, .value = lhs.value },
			rhs.value
		);
	}
	else if (expr->oper == EXPR_POW) {
		result = gen_expr_pow(&lhs, rhs.value, err);
	}

	if ((!result.type && !result.value) || *err) return (Expr){0};

	if (type && result.type != type) {
		const Token *lhs_token = expr->lhs.expr ?
			(expr->lhs.expr->oper == EXPR_FUNC ?
				expr->lhs.expr->lhs.func_call->func_name_tok :
				expr->lhs.expr->lhs.tok) :
				NULL;

		const Token *tok = lhs_token ? lhs_token : rhs_token;

		FMT_ERROR(ERR_EXPECTED_SAME_TYPE,
			{ .loc = &tok->location, .type = type },
			{ .type = result.type }
		);

		*err = true;
		return (Expr){0};
	}

	return result;
}

/*
Return expression for identifier `token` with type `type`.

If `type` is not set, the expression type will not be checked.

Set `err` if an error occurred.
*/
static Expr gen_expr_identifier(
	Type type,
	AstNodeExpr *expr_node,
	bool *err
) {
	Variable *var = NULL;
	const Expr expr = ident_to_expr(expr_node, &var);
	if (!expr.value && !expr.type) {
		*err = true;
		return (Expr){0};
	}

	if (type && var && var->type != type) {
		FMT_ERROR(ERR_EXPECTED_SAME_TYPE,
			{ .loc = &expr_node->lhs.tok->location, .type = type },
			{ .type = var->type }
		);

		variable_disable_warnings(var);

		*err = true;
		return (Expr){0};
	}

	return expr;
}

/*
Convert identifier node `expr` to an expression.

Store found variable (if found) in `variable`.
*/
static Expr ident_to_expr(
	AstNodeExpr *expr,
	Variable **variable
) {
	Variable *var_found = expr->var;
	var_found->was_read = true;

	if (variable) *variable = var_found;

	if (var_found->is_const &&
		!(var_found->is_global &&
		!var_found->is_const_lit)
	) {
		return (Expr){
			.value = var_found->ref,
			.type = var_found->type
		};
	}

	return (Expr) {
		.value = LLVMBuildLoad2(
			SKULL_STATE_LLVM.builder,
			type_to_llvm_type(var_found->type),
			var_found->ref,
			""
		),
		.type = var_found->type
	};
}

/*
Make a simple expression (const literal) from `expr`.
*/
static Expr gen_expr_const(const AstNodeExpr *expr) {
	LLVMValueRef value = NULL;
	Type type = NULL;
	const Token *token = expr->lhs.tok;

	if (token->type == TOKEN_INT_CONST) {
		value = LLVM_INT(expr->value._int);
		type = TYPE_INT;
	}
	else if (token->type == TOKEN_FLOAT_CONST) {
		value = LLVM_FLOAT(expr->value._float);
		type = TYPE_FLOAT;
	}
	else if (token->type == TOKEN_BOOL_CONST) {
		value = LLVM_BOOL(expr->value._bool);
		type = TYPE_BOOL;
	}
	else if (token->type == TOKEN_RUNE_CONST) {
		value = LLVM_RUNE(expr->value.rune);
		type = TYPE_RUNE;
	}
	else if (token->type == TOKEN_STR_CONST) {
		value = LLVMBuildBitCast(
			SKULL_STATE_LLVM.builder,
			LLVMBuildGlobalString(
				SKULL_STATE_LLVM.builder,
				expr->value.str,
				""
			),
			type_to_llvm_type(TYPE_STR),
			""
		);

		type = TYPE_STR;
	}

	return (Expr){
		.value = value,
		.type = type
	};
}

/*
Returns the result of a mathematical operation on `lhs` and `rhs`.

Depending on whether `type` is an int or float, combine `lhs` and `rhs`
using `int_func` or `float_func`.
*/
static Expr gen_expr_math_oper(
	const Expr *lhs,
	LLVMValueRef rhs,
	LLVMBuildX int_func,
	LLVMBuildX float_func
) {
	if (lhs->type == TYPE_INT)
		return (Expr){
			.value = int_func(SKULL_STATE_LLVM.builder, lhs->value, rhs, ""),
			.type = lhs->type
		};

	if (lhs->type == TYPE_FLOAT)
		return (Expr){
			.value = float_func(SKULL_STATE_LLVM.builder, lhs->value, rhs, ""),
			.type = lhs->type
		};

	return (Expr){0};
}

/*
Return expression for addition of `lhs` and `rhs`.
*/
static Expr gen_expr_add(
	const Expr *lhs,
	LLVMValueRef rhs
) {
	return gen_expr_math_oper(lhs, rhs, LLVMBuildNSWAdd, LLVMBuildFAdd);
}

/*
Return expression for subtraction of `lhs` and `rhs`.
*/
static Expr gen_expr_sub(
	const Expr *lhs,
	LLVMValueRef rhs
) {
	return gen_expr_math_oper(lhs, rhs, LLVMBuildNSWSub, LLVMBuildFSub);
}

/*
Return expression for multiplication of `lhs` and `rhs`.
*/
static Expr gen_expr_mult(
	const Expr *lhs,
	LLVMValueRef rhs
) {
	return gen_expr_math_oper(lhs, rhs, LLVMBuildNSWMul, LLVMBuildFMul);
}

/*
Return expression for division of `lhs` and `rhs`.
*/
static Expr gen_expr_div(
	const Expr *lhs,
	LLVMValueRef rhs
) {
	return gen_expr_math_oper(lhs, rhs, LLVMBuildExactSDiv, LLVMBuildFDiv);
}

/*
Return expression for modulus of `lhs` and `rhs`.
*/
static Expr gen_expr_mod(
	const Expr *lhs,
	LLVMValueRef rhs
) {
	return gen_expr_math_oper(lhs, rhs, LLVMBuildSRem, LLVMBuildFRem);
}

/*
Return expression for left shift of `lhs` and `rhs`.
*/
static Expr gen_expr_lshift(
	const Expr *lhs,
	LLVMValueRef rhs
) {
	if (lhs->type == TYPE_INT)
		return (Expr){
			.value = LLVMBuildShl(
				SKULL_STATE_LLVM.builder,
				lhs->value,
				rhs,
				""
			),
			.type = TYPE_INT
		};

	return (Expr){0};
}

/*
Return expression for logical right shift of `lhs` and `rhs`.
*/
static Expr gen_expr_rshift(
	const Expr *lhs,
	LLVMValueRef rhs
) {
	if (lhs->type == TYPE_INT)
		return (Expr){
			.value = LLVMBuildLShr(
				SKULL_STATE_LLVM.builder,
				lhs->value,
				rhs,
				""
			),
			.type = TYPE_INT
		};

	return (Expr){0};
}

/*
Return expression for taking `lhs` to the power of `rhs`.

Set `err` if an error occurred.
*/
static Expr gen_expr_pow(
	const Expr *lhs,
	LLVMValueRef rhs,
	bool *err
) {
	const char *func_name = NULL;

	if (lhs->type == TYPE_INT)
		func_name = "_int_pow";

	else if (lhs->type == TYPE_FLOAT)
		func_name = "_float_pow";

	else {
		FMT_ERROR(ERR_POW_BAD_TYPE, { .type = lhs->type });

		*err = true;
		return (Expr){0};
	}

	return create_and_call_builtin_oper(
		lhs->type,
		type_to_llvm_type(lhs->type),
		func_name,
		lhs->value,
		rhs
	);
}

/*
Return expression for result of not operator for `rhs`.
*/
static Expr gen_expr_not(const Expr *lhs, LLVMValueRef rhs) {
	if (lhs->type != TYPE_BOOL) return (Expr){0};

	return (Expr){
		.value = LLVMBuildNot(SKULL_STATE_LLVM.builder, rhs, ""),
		.type = TYPE_BOOL
	};
}

/*
Return expression for result of unary negation operator for `rhs`.
*/
static Expr gen_expr_unary_neg(const Expr *lhs, LLVMValueRef rhs) {
	return gen_expr_math_oper(
		&(Expr){
			.type = lhs->type,
			.value = lhs->type == TYPE_INT ?
				LLVM_INT(0) :
				LLVM_FLOAT(0)
		},
		rhs,
		LLVMBuildNSWSub,
		LLVMBuildFSub
	);
}

/*
Return expression for result of is operator for `lhs` and `rhs`.
*/
static Expr gen_expr_is(const Expr *lhs, LLVMValueRef rhs) {
	Type type = lhs->type;

	if (type == TYPE_INT || type == TYPE_RUNE || type == TYPE_BOOL)
		return (Expr){
			.value = LLVMBuildICmp(
				SKULL_STATE_LLVM.builder,
				LLVMIntEQ,
				lhs->value,
				rhs,
				""
			),
			.type = TYPE_BOOL
		};

	if (type == TYPE_FLOAT)
		return (Expr){
			.value = LLVMBuildFCmp(
				SKULL_STATE_LLVM.builder,
				LLVMRealOEQ,
				lhs->value,
				rhs,
				""
			),
			.type = TYPE_BOOL
		};

	if (type == TYPE_STR)
		return gen_expr_is_str(lhs->value, rhs);

	return (Expr){0};
}

/*
Return expression for string-is operator against `lhs` and `rhs`.
*/
static Expr gen_expr_is_str(LLVMValueRef lhs, LLVMValueRef rhs) {
	return create_and_call_builtin_oper(
		TYPE_BOOL,
		type_to_llvm_type(TYPE_STR),
		"_strcmp",
		lhs,
		rhs
	);
}

/*
Create a function called `name` (if it does not exist) which returns type
`rtype`, and has operands of type `type`. Afterwards, call the new function
with the `lhs` and `rhs` operands.
*/
static Expr create_and_call_builtin_oper(
	Type rtype,
	LLVMTypeRef type,
	const char *name,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	LLVMValueRef func = LLVMGetNamedFunction(SKULL_STATE_LLVM.module, name);

	LLVMTypeRef func_type = type_to_llvm_func_type(
		rtype,
		(LLVMTypeRef[]){ type, type },
		2
	);

	if (!func)
		func = LLVMAddFunction(
			SKULL_STATE_LLVM.module,
			name,
			func_type
		);

	return (Expr){
		.value = LLVMBuildCall2(
			SKULL_STATE_LLVM.builder,
			func_type,
			func,
			(LLVMValueRef[]){ lhs, rhs },
			2,
			""
		),
		.type = rtype
	};
}

/*
Return expression for result of is not operator for `lhs` and `rhs`.
*/
static Expr gen_expr_is_not(
	const Expr *lhs,
	LLVMValueRef rhs
) {
	Expr expr = gen_expr_is(lhs, rhs);

	expr.value = LLVMBuildNot(
		SKULL_STATE_LLVM.builder,
		expr.value,
		""
	);

	return expr;
}

/*
Return result of relational comparison on `lhs` and `rhs`.

Depending on whether `type` is an int or float, compare `lhs` and `rhs` using
`int_func` or `float_func`.
*/
static Expr gen_expr_relational_oper(
	const Expr *lhs,
	LLVMValueRef rhs,
	LLVMIntPredicate int_pred,
	LLVMRealPredicate float_pred
) {
	if (lhs->type == TYPE_INT)
		return (Expr){
			.value = LLVMBuildICmp(
				SKULL_STATE_LLVM.builder,
				int_pred,
				lhs->value,
				rhs,
				""
			),
			.type = TYPE_BOOL
		};

	if (lhs->type == TYPE_FLOAT)
		return (Expr){
			.value = LLVMBuildFCmp(
				SKULL_STATE_LLVM.builder,
				float_pred,
				lhs->value,
				rhs,
				""
			),
			.type = TYPE_BOOL
		};

	return (Expr){0};
}

/*
Return expression for result of less than operator for `lhs` and `rhs`.
*/
static Expr gen_expr_less_than(
	const Expr *lhs,
	LLVMValueRef rhs
) {
	return gen_expr_relational_oper(lhs, rhs, LLVMIntSLT, LLVMRealOLT);
}

/*
Return expression for result of greater than operator for `lhs` and `rhs`.
*/
static Expr gen_expr_gtr_than(
	const Expr *lhs,
	LLVMValueRef rhs
) {
	return gen_expr_relational_oper(lhs, rhs, LLVMIntSGT, LLVMRealOGT);
}

/*
Return expression for result of less than or equal to operator for `lhs` and
`rhs`.
*/
static Expr gen_expr_less_than_eq(
	const Expr *lhs,
	LLVMValueRef rhs
) {
	return gen_expr_relational_oper(lhs, rhs, LLVMIntSLE, LLVMRealOLE);
}

/*
Return expression for result of greater than or equal to operator for `lhs`
and `rhs`.
*/
static Expr gen_expr_gtr_than_eq(
	const Expr *lhs,
	LLVMValueRef rhs
) {
	return gen_expr_relational_oper(lhs, rhs, LLVMIntSGE, LLVMRealOGE);
}

/*
Return result of logical operation `func` on `lhs` and `rhs`.
*/
static Expr gen_expr_logical_oper(
	const Expr *lhs,
	LLVMValueRef rhs,
	LLVMBuildX func
) {
	return (Expr){
		.value = func(
			SKULL_STATE_LLVM.builder,
			lhs->value,
			rhs,
			""
		),
		.type = lhs->type
	};
}

/*
Return result of logical "and" operation of `lhs` and `rhs`.
*/
static Expr gen_expr_and(
	const Expr *lhs,
	LLVMValueRef rhs
) {
	return gen_expr_logical_oper(lhs, rhs, LLVMBuildAnd);
}

/*
Return result of logical "or" operation of `lhs` and `rhs`.
*/
static Expr gen_expr_or(const Expr *lhs, LLVMValueRef rhs) {
	return gen_expr_logical_oper(lhs, rhs, LLVMBuildOr);
}

/*
Return result of logical "xor" operation of `lhs` and `rhs`.
*/
static Expr gen_expr_xor(
	const Expr *lhs,
	LLVMValueRef rhs
) {
	return gen_expr_logical_oper(lhs, rhs, LLVMBuildXor);
}

static Operation *expr_type_to_func(ExprType oper) {
	switch (oper) {
		case EXPR_ADD: return gen_expr_add;
		case EXPR_SUB: return gen_expr_sub;
		case EXPR_UNARY_NEG: return gen_expr_unary_neg;
		case EXPR_MULT: return gen_expr_mult;
		case EXPR_NOT: return gen_expr_not;
		case EXPR_LSHIFT: return gen_expr_lshift;
		case EXPR_RSHIFT: return gen_expr_rshift;
		case EXPR_IS: return gen_expr_is;
		case EXPR_ISNT: return gen_expr_is_not;
		case EXPR_LESS_THAN: return gen_expr_less_than;
		case EXPR_GTR_THAN: return gen_expr_gtr_than;
		case EXPR_LESS_THAN_EQ: return gen_expr_less_than_eq;
		case EXPR_GTR_THAN_EQ: return gen_expr_gtr_than_eq;
		case EXPR_AND: return gen_expr_and;
		case EXPR_OR: return gen_expr_or;
		case EXPR_XOR: return gen_expr_xor;
		case EXPR_MOD: return gen_expr_mod;
		case EXPR_DIV: return gen_expr_div;
		default: return NULL;
	}
}

static bool is_bool_expr(ExprType oper) {
	switch (oper) {
		case EXPR_IS:
		case EXPR_ISNT:
		case EXPR_LESS_THAN:
		case EXPR_GTR_THAN:
		case EXPR_LESS_THAN_EQ:
		case EXPR_GTR_THAN_EQ:
		case EXPR_AND:
		case EXPR_OR:
		case EXPR_XOR:
			return true;
		default: return false;
	}
}
