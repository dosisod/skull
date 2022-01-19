#include <assert.h>
#include <stdbool.h>

#include <llvm-c/Core.h>

#include "skull/codegen/llvm/aliases.h"
#include "skull/codegen/llvm/func.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/codegen/llvm/types.h"
#include "skull/common/str.h"
#include "skull/parse/ast_node.h"
#include "skull/semantic/scope.h"

#include "skull/codegen/llvm/expr.h"

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
	gen_expr_and, gen_expr_or, gen_expr_xor, gen_expr_div, gen_expr_mod,
	gen_expr_pow;

static Expr gen_expr_const(const AstNodeExpr *);
static Expr gen_expr_is_str(LLVMValueRef, LLVMValueRef);
static Expr gen_expr_identifier(const AstNodeExpr *);
static Expr gen_expr_ref(const AstNodeExpr *);
static Expr gen_expr_deref(const AstNodeExpr *);
static Operation *expr_type_to_func(ExprType);

static Expr create_and_call_builtin_oper(
	const Type *,
	LLVMTypeRef,
	const char *,
	LLVMValueRef,
	LLVMValueRef
);

/*
Return expression for `expr`.
*/
Expr gen_expr(const AstNodeExpr *const expr) {
	switch (expr->oper) {
		case EXPR_IDENTIFIER:
			return gen_expr_identifier(expr);
		case EXPR_CONST:
			return gen_expr_const(expr);
		case EXPR_FUNC:
			return gen_expr_func_call(expr->lhs.func_call);
		case EXPR_REF:
			return gen_expr_ref(expr);
		case EXPR_DEREF:
			return gen_expr_deref(expr);
		default: break;
	}

	const Expr lhs = expr->lhs.expr ? gen_expr(expr->lhs.expr) : (Expr){0};
	const Expr rhs = gen_expr(expr->rhs);
	Operation *func = expr_type_to_func(expr->oper);

	return func(
		&(Expr){ .type = rhs.type, .value = lhs.value },
		rhs.value
	);
}

/*
Return expression for identifier `token` with type `type`.
*/
static Expr gen_expr_identifier(const AstNodeExpr *expr) {
	const Variable *var = expr->var;

	if (var->is_global || !var->is_const) {
		return (Expr) {
			.value = LLVMBuildLoad2(
				SKULL_STATE_LLVM.builder,
				type_to_llvm_type(var->type),
				var->ref,
				""
			),
			.type = var->type
		};
	}

	return (Expr){
		.value = var->ref,
		.type = var->type
	};
}

static Expr gen_expr_deref(const AstNodeExpr *expr) {
	const Variable *var = expr->rhs->var;

	return (Expr) {
		.value = LLVMBuildLoad2(
			SKULL_STATE_LLVM.builder,
			type_to_llvm_type(var->type->inner),
			LLVMBuildLoad2(
				SKULL_STATE_LLVM.builder,
				type_to_llvm_type(var->type),
				var->ref,
				""
			),
			""
		),
		.type = var->type->inner
	};
}

/*
Make a simple expression (const literal) from `expr`.
*/
static Expr gen_expr_const(const AstNodeExpr *expr) {
	LLVMValueRef value = NULL;

	if (expr->type == &TYPE_INT) {
		value = LLVM_INT(expr->value._int);
	}
	else if (expr->type == &TYPE_FLOAT) {
		value = LLVM_FLOAT(expr->value._float);
	}
	else if (expr->type == &TYPE_BOOL) {
		value = LLVM_BOOL(expr->value._bool);
	}
	else if (expr->type == &TYPE_RUNE) {
		value = LLVM_RUNE(expr->value.rune);
	}
	else if (expr->type == &TYPE_STR) {
		value = LLVMBuildBitCast(
			SKULL_STATE_LLVM.builder,
			LLVMBuildGlobalString(
				SKULL_STATE_LLVM.builder,
				expr->value.str,
				""
			),
			type_to_llvm_type(&TYPE_STR),
			""
		);
	}

	return (Expr){
		.value = value,
		.type = expr->type
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
	if (lhs->type == &TYPE_INT)
		return (Expr){
			.value = int_func(SKULL_STATE_LLVM.builder, lhs->value, rhs, ""),
			.type = lhs->type
		};

	if (lhs->type == &TYPE_FLOAT)
		return (Expr){
			.value = float_func(SKULL_STATE_LLVM.builder, lhs->value, rhs, ""),
			.type = lhs->type
		};

	assert(false);
}

/*
Return expression for addition of `lhs` and `rhs`.
*/
static Expr gen_expr_add(const Expr *lhs, LLVMValueRef rhs) {
	return gen_expr_math_oper(lhs, rhs, LLVMBuildNSWAdd, LLVMBuildFAdd);
}

/*
Return expression for subtraction of `lhs` and `rhs`.
*/
static Expr gen_expr_sub(const Expr *lhs, LLVMValueRef rhs) {
	return gen_expr_math_oper(lhs, rhs, LLVMBuildNSWSub, LLVMBuildFSub);
}

/*
Return expression for multiplication of `lhs` and `rhs`.
*/
static Expr gen_expr_mult(const Expr *lhs, LLVMValueRef rhs) {
	return gen_expr_math_oper(lhs, rhs, LLVMBuildNSWMul, LLVMBuildFMul);
}

/*
Return expression for division of `lhs` and `rhs`.
*/
static Expr gen_expr_div(const Expr *lhs, LLVMValueRef rhs) {
	return gen_expr_math_oper(lhs, rhs, LLVMBuildExactSDiv, LLVMBuildFDiv);
}

/*
Return expression for modulus of `lhs` and `rhs`.
*/
static Expr gen_expr_mod(const Expr *lhs, LLVMValueRef rhs) {
	return gen_expr_math_oper(lhs, rhs, LLVMBuildSRem, LLVMBuildFRem);
}

/*
Return expression for left shift of `lhs` and `rhs`.
*/
static Expr gen_expr_lshift(const Expr *lhs, LLVMValueRef rhs) {
	if (lhs->type == &TYPE_INT)
		return (Expr){
			.value = LLVMBuildShl(
				SKULL_STATE_LLVM.builder,
				lhs->value,
				rhs,
				""
			),
			.type = &TYPE_INT
		};

	assert(false);
}

/*
Return expression for logical right shift of `lhs` and `rhs`.
*/
static Expr gen_expr_rshift(const Expr *lhs, LLVMValueRef rhs) {
	if (lhs->type == &TYPE_INT)
		return (Expr){
			.value = LLVMBuildLShr(
				SKULL_STATE_LLVM.builder,
				lhs->value,
				rhs,
				""
			),
			.type = &TYPE_INT
		};

	assert(false);
}

/*
Return expression for taking `lhs` to the power of `rhs`.
*/
static Expr gen_expr_pow(const Expr *lhs, LLVMValueRef rhs) {
	const char *func_name = NULL;

	if (lhs->type == &TYPE_INT)
		func_name = "_int_pow";

	else if (lhs->type == &TYPE_FLOAT)
		func_name = "_float_pow";

	return create_and_call_builtin_oper(
		lhs->type,
		type_to_llvm_type(lhs->type),
		func_name,
		lhs->value,
		rhs
	);
}

/*
Return reference to an expression `expr`.
*/
static Expr gen_expr_ref(const AstNodeExpr *expr) {
	return (Expr){
		.value = expr->rhs->var->ref,
		.type = expr->rhs->var->type->inner
	};
}

/*
Return expression for result of not operator for `rhs`.
*/
static Expr gen_expr_not(const Expr *lhs, LLVMValueRef rhs) {
	(void)lhs;

	return (Expr){
		.value = LLVMBuildNot(SKULL_STATE_LLVM.builder, rhs, ""),
		.type = &TYPE_BOOL
	};
}

/*
Return expression for result of unary negation operator for `rhs`.
*/
static Expr gen_expr_unary_neg(const Expr *lhs, LLVMValueRef rhs) {
	return gen_expr_math_oper(
		&(Expr){
			.type = lhs->type,
			.value = lhs->type == &TYPE_INT ?
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
	const Type *type = lhs->type;

	if (type == &TYPE_INT || type == &TYPE_RUNE || type == &TYPE_BOOL)
		return (Expr){
			.value = LLVMBuildICmp(
				SKULL_STATE_LLVM.builder,
				LLVMIntEQ,
				lhs->value,
				rhs,
				""
			),
			.type = &TYPE_BOOL
		};

	if (type == &TYPE_FLOAT)
		return (Expr){
			.value = LLVMBuildFCmp(
				SKULL_STATE_LLVM.builder,
				LLVMRealOEQ,
				lhs->value,
				rhs,
				""
			),
			.type = &TYPE_BOOL
		};

	if (type == &TYPE_STR)
		return gen_expr_is_str(lhs->value, rhs);

	assert(false);
}

/*
Return expression for string-is operator against `lhs` and `rhs`.
*/
static Expr gen_expr_is_str(LLVMValueRef lhs, LLVMValueRef rhs) {
	return create_and_call_builtin_oper(
		&TYPE_BOOL,
		type_to_llvm_type(&TYPE_STR),
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
	const Type *rtype,
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
	if (lhs->type == &TYPE_INT)
		return (Expr){
			.value = LLVMBuildICmp(
				SKULL_STATE_LLVM.builder,
				int_pred,
				lhs->value,
				rhs,
				""
			),
			.type = &TYPE_BOOL
		};

	if (lhs->type == &TYPE_FLOAT)
		return (Expr){
			.value = LLVMBuildFCmp(
				SKULL_STATE_LLVM.builder,
				float_pred,
				lhs->value,
				rhs,
				""
			),
			.type = &TYPE_BOOL
		};

	assert(false);
}

/*
Return expression for result of less than operator for `lhs` and `rhs`.
*/
static Expr gen_expr_less_than(const Expr *lhs, LLVMValueRef rhs) {
	return gen_expr_relational_oper(lhs, rhs, LLVMIntSLT, LLVMRealOLT);
}

/*
Return expression for result of greater than operator for `lhs` and `rhs`.
*/
static Expr gen_expr_gtr_than(const Expr *lhs, LLVMValueRef rhs) {
	return gen_expr_relational_oper(lhs, rhs, LLVMIntSGT, LLVMRealOGT);
}

/*
Return expression for result of less than or equal to operator for `lhs` and
`rhs`.
*/
static Expr gen_expr_less_than_eq(const Expr *lhs, LLVMValueRef rhs) {
	return gen_expr_relational_oper(lhs, rhs, LLVMIntSLE, LLVMRealOLE);
}

/*
Return expression for result of greater than or equal to operator for `lhs`
and `rhs`.
*/
static Expr gen_expr_gtr_than_eq(const Expr *lhs, LLVMValueRef rhs) {
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
static Expr gen_expr_and(const Expr *lhs, LLVMValueRef rhs) {
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
static Expr gen_expr_xor(const Expr *lhs, LLVMValueRef rhs) {
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
		case EXPR_POW: return gen_expr_pow;
		default: return NULL;
	}
}
