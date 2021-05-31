#include <stdbool.h>

#include <llvm-c/Core.h>

#include "skull/codegen/llvm/aliases.h"
#include "skull/codegen/llvm/types.h"
#include "skull/codegen/scope.h"
#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/compiler/types/bool.h"
#include "skull/compiler/types/float.h"
#include "skull/compiler/types/int.h"
#include "skull/compiler/types/rune.h"
#include "skull/compiler/types/str.h"
#include "skull/parse/ast_node.h"

#include "skull/codegen/expr.h"

typedef Expr (Operation)(Type, LLVMValueRef, LLVMValueRef);
typedef Expr (OperationWithErr)(Type, LLVMValueRef, LLVMValueRef, bool *);

static Operation gen_expr_add;
static Operation gen_expr_sub;
static Operation gen_expr_mult;
static OperationWithErr gen_expr_div;
static OperationWithErr gen_expr_mod;
static Operation gen_expr_not;
static Operation gen_expr_unary_neg;
static Operation gen_expr_is;
static Operation gen_expr_is_not;
static Operation gen_expr_less_than;
static Operation gen_expr_gtr_than;
static Operation gen_expr_less_than_eq;
static Operation gen_expr_gtr_than_eq;
static Operation gen_expr_lshift;
static Operation gen_expr_rshift;
static Operation gen_expr_and;
static Operation gen_expr_or;
static Operation gen_expr_xor;
static OperationWithErr gen_expr_pow;

static Expr token_to_expr(const Token *const, Variable **, bool *);
static Expr gen_expr_const(Type, const Token *const, bool *);
static Expr gen_expr_is_str(LLVMValueRef, LLVMValueRef);
static Expr token_to_simple_expr(const Token *const, bool *err);
static Expr gen_expr(Type, const AstNodeExpr *const, _Bool *);

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


static Expr gen_expr_identifier(
	Type,
	const Token *const,
	bool *
);

typedef LLVMValueRef (LLVMBuildX)(
	LLVMBuilderRef,
	LLVMValueRef,
	LLVMValueRef,
	const char *
);

static Expr create_and_call_builtin_oper(
	Type,
	LLVMTypeRef,
	const char *,
	LLVMValueRef,
	LLVMValueRef
);

/*
Returns the result of a mathematical operation on `lhs` and `rhs`.

Depending on whether `type` is an int or float, combine `lhs` and `rhs`
using `int_func` or `float_func`.
*/
static Expr gen_expr_math_oper(
	Type type,
	LLVMValueRef lhs,
	LLVMValueRef rhs,
	LLVMBuildX int_func,
	LLVMBuildX float_func
) {
	if (type == TYPE_INT)
		return (Expr){
			.value = int_func(SKULL_STATE.builder, lhs, rhs, ""),
			.type = type
		};

	if (type == TYPE_FLOAT)
		return (Expr){
			.value = float_func(SKULL_STATE.builder, lhs, rhs, ""),
			.type = type
		};

	return (Expr){0};
}

/*
Return expression for addition of `lhs` and `rhs`.
*/
static Expr gen_expr_add(
	Type type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	return gen_expr_math_oper(type, lhs, rhs, LLVMBuildNSWAdd, LLVMBuildFAdd);
}

/*
Return expression for subtraction of `lhs` and `rhs`.
*/
static Expr gen_expr_sub(
	Type type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	return gen_expr_math_oper(type, lhs, rhs, LLVMBuildNSWSub, LLVMBuildFSub);
}

/*
Return expression for multiplication of `lhs` and `rhs`.
*/
static Expr gen_expr_mult(
	Type type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	return gen_expr_math_oper(type, lhs, rhs, LLVMBuildNSWMul, LLVMBuildFMul);
}

/*
Return expression for division of `lhs` and `rhs`.

Set `err` if an error occurred.
*/
static Expr gen_expr_div(
	Type type,
	LLVMValueRef lhs,
	LLVMValueRef rhs,
	bool *err
) {
	if (type == TYPE_INT) {
		if (LLVMConstIntGetSExtValue(rhs) == 0) {
			FMT_ERROR(ERR_DIV_BY_ZERO, {0});

			*err = true;
			return (Expr){0};
		}
	}

	return gen_expr_math_oper(
		type,
		lhs,
		rhs,
		LLVMBuildExactSDiv,
		LLVMBuildFDiv
	);
}

/*
Return expression for modulus of `lhs` and `rhs`.

Set `err` if an error occurred.
*/
static Expr gen_expr_mod(
	Type type,
	LLVMValueRef lhs,
	LLVMValueRef rhs,
	bool *err
) {
	if (type == TYPE_INT) {
		if (LLVMConstIntGetSExtValue(rhs) == 0) {
			FMT_ERROR(ERR_DIV_BY_ZERO, {0});

			*err = true;
			return (Expr){0};
		}
	}

	return gen_expr_math_oper(
		type,
		lhs,
		rhs,
		LLVMBuildSRem,
		LLVMBuildFRem
	);
}

/*
Return expression for left shift of `lhs` and `rhs`.
*/
static Expr gen_expr_lshift(
	Type type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	if (type == TYPE_INT)
		return (Expr){
			.value = LLVMBuildShl(SKULL_STATE.builder, lhs, rhs, ""),
			.type = TYPE_INT
		};

	return (Expr){0};
}

/*
Return expression for logical right shift of `lhs` and `rhs`.
*/
static Expr gen_expr_rshift(
	Type type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	if (type == TYPE_INT)
		return (Expr){
			.value = LLVMBuildLShr(SKULL_STATE.builder, lhs, rhs, ""),
			.type = TYPE_INT
		};

	return (Expr){0};
}

/*
Return expression for taking `lhs` to the power of `rhs`.

Set `err` if an error occurred.
*/
static Expr gen_expr_pow(
	Type type,
	LLVMValueRef lhs,
	LLVMValueRef rhs,
	bool *err
) {
	const char *func_name = NULL;

	if (type == TYPE_INT)
		func_name = "_int_pow";

	else if (type == TYPE_FLOAT)
		func_name = "_float_pow";

	else {
		FMT_ERROR(ERR_POW_BAD_TYPE, { .type = type });

		*err = true;
		return (Expr){0};
	}

	return create_and_call_builtin_oper(
		type,
		gen_llvm_type(type),
		func_name,
		lhs,
		rhs
	);
}

/*
Return expression for result of not operator for `rhs`.
*/
static Expr gen_expr_not(Type type, LLVMValueRef lhs, LLVMValueRef rhs) {
	(void)type;
	(void)lhs;

	return (Expr){
		.value = LLVMBuildNot(SKULL_STATE.builder, rhs, ""),
		.type = TYPE_BOOL
	};
}

/*
Return expression for result of unary negation operator for `rhs`.
*/
static Expr gen_expr_unary_neg(Type type, LLVMValueRef lhs, LLVMValueRef rhs) {
	(void)lhs;

	return gen_expr_math_oper(
		type,
		LLVM_INT(0),
		rhs,
		LLVMBuildNSWSub,
		LLVMBuildFSub
	);
}

/*
Return expression for result of is operator for `lhs` and `rhs`.
*/
static Expr gen_expr_is(Type type, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (type == TYPE_INT || type == TYPE_RUNE || type == TYPE_BOOL)
		return (Expr){
			.value = LLVMBuildICmp(
				SKULL_STATE.builder,
				LLVMIntEQ,
				lhs,
				rhs,
				""
			),
			.type = TYPE_BOOL
		};

	if (type == TYPE_FLOAT)
		return (Expr){
			.value = LLVMBuildFCmp(
				SKULL_STATE.builder,
				LLVMRealOEQ,
				lhs,
				rhs,
				""
			),
			.type = TYPE_BOOL
		};

	if (type == TYPE_STR)
		return gen_expr_is_str(lhs, rhs);

	return (Expr){0};
}

/*
Return expression for string-is operator against `lhs` and `rhs`.
*/
static Expr gen_expr_is_str(LLVMValueRef lhs, LLVMValueRef rhs) {
	return create_and_call_builtin_oper(
		TYPE_BOOL,
		gen_llvm_type(TYPE_STR),
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
	LLVMValueRef func = LLVMGetNamedFunction(SKULL_STATE.module, name);

	LLVMTypeRef func_type = gen_llvm_func_type(
		rtype,
		(LLVMTypeRef[]){ type, type },
		2
	);

	if (!func)
		func = LLVMAddFunction(
			SKULL_STATE.module,
			name,
			func_type
		);

	return (Expr){
		.value = LLVMBuildCall2(
			SKULL_STATE.builder,
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
	Type type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	Expr expr = gen_expr_is(type, lhs, rhs);

	expr.value = LLVMBuildNot(
		SKULL_STATE.builder,
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
	Type type,
	LLVMValueRef lhs,
	LLVMValueRef rhs,
	LLVMIntPredicate int_pred,
	LLVMRealPredicate float_pred
) {
	if (type == TYPE_INT)
		return (Expr){
			.value = LLVMBuildICmp(
				SKULL_STATE.builder,
				int_pred,
				lhs,
				rhs,
				""
			),
			.type = TYPE_BOOL
		};

	if (type == TYPE_FLOAT)
		return (Expr){
			.value = LLVMBuildFCmp(
				SKULL_STATE.builder,
				float_pred,
				lhs,
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
	Type type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	return gen_expr_relational_oper(type, lhs, rhs, LLVMIntSLT, LLVMRealOLT);
}

/*
Return expression for result of greater than operator for `lhs` and `rhs`.
*/
static Expr gen_expr_gtr_than(
	Type type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	return gen_expr_relational_oper(type, lhs, rhs, LLVMIntSGT, LLVMRealOGT);
}

/*
Return expression for result of less than or equal to operator for `lhs` and
`rhs`.
*/
static Expr gen_expr_less_than_eq(
	Type type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	return gen_expr_relational_oper(type, lhs, rhs, LLVMIntSLE, LLVMRealOLE);
}

/*
Return expression for result of greater than or equal to operator for `lhs`
and `rhs`.
*/
static Expr gen_expr_gtr_than_eq(
	Type type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	return gen_expr_relational_oper(type, lhs, rhs, LLVMIntSGE, LLVMRealOGE);
}

/*
Return result of logical operation `func` on `lhs` and `rhs`.
*/
static Expr gen_expr_logical_oper(
	Type type,
	LLVMValueRef lhs,
	LLVMValueRef rhs,
	LLVMBuildX func
) {
	return (Expr){
		.value = func(
			SKULL_STATE.builder,
			lhs,
			rhs,
			""
		),
		.type = type
	};
}

/*
Return result of logical "and" operation of `lhs` and `rhs`.
*/
static Expr gen_expr_and(
	Type type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	return gen_expr_logical_oper(type, lhs, rhs, LLVMBuildAnd);
}

/*
Return result of logical "or" operation of `lhs` and `rhs`.
*/
static Expr gen_expr_or(Type type, LLVMValueRef lhs, LLVMValueRef rhs) {
	return gen_expr_logical_oper(type, lhs, rhs, LLVMBuildOr);
}

/*
Return result of logical "xor" operation of `lhs` and `rhs`.
*/
static Expr gen_expr_xor(
	Type type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	return gen_expr_logical_oper(type, lhs, rhs, LLVMBuildXor);
}

/*
Return expression for identifier `token` with type `type`.

Optionally pass `var` if result is expected to be assigned to a variable.

If `type` is not set, the expression type will not be checked.

Set `err` if an error occurred.
*/
static Expr gen_expr_identifier(
	Type type,
	const Token *const token,
	bool *err
) {
	Variable *var = NULL;
	const Expr expr = token_to_expr(token, &var, err);
	if (*err) return (Expr){0};

	if (type && var && var->type != type) {
		FMT_ERROR(ERR_EXPECTED_SAME_TYPE,
			{ .loc = &token->location, .type = type },
			{ .type = var->type }
		);

		variable_no_warnings(var);

		*err = true;
		return (Expr){0};
	}

	return expr;
}

/*
Return expression for operation `oper` for `expr`.

Set `err` if an error occurred.
*/
static Expr gen_expr(
	Type type,
	const AstNodeExpr *const expr,
	bool *err
) {
	Operation *func = NULL;
	OperationWithErr *func_with_err = NULL;

	// true if expr results in different type then its operands
	bool is_diff_type = false;

	switch (expr->oper) {
		case EXPR_ADD: func = &gen_expr_add; break;
		case EXPR_SUB: func = &gen_expr_sub; break;
		case EXPR_UNARY_NEG: func = &gen_expr_unary_neg; break;
		case EXPR_MULT: func = &gen_expr_mult; break;
		case EXPR_DIV: func_with_err = &gen_expr_div; break;
		case EXPR_MOD: func_with_err = &gen_expr_mod; break;
		case EXPR_NOT: func = &gen_expr_not; break;
		case EXPR_LSHIFT: func = &gen_expr_lshift; break;
		case EXPR_POW: func_with_err = &gen_expr_pow; break;
		case EXPR_RSHIFT: func = &gen_expr_rshift; break;
		case EXPR_IS: func = gen_expr_is; is_diff_type = true; break;
		case EXPR_ISNT: func = gen_expr_is_not; is_diff_type = true; break;
		case EXPR_LESS_THAN:
			func = gen_expr_less_than; is_diff_type = true; break;
		case EXPR_GTR_THAN:
			func = gen_expr_gtr_than; is_diff_type = true; break;
		case EXPR_LESS_THAN_EQ:
			func = gen_expr_less_than_eq; is_diff_type = true; break;
		case EXPR_GTR_THAN_EQ:
			func = gen_expr_gtr_than_eq; is_diff_type = true; break;
		case EXPR_AND: func = gen_expr_and; is_diff_type = true; break;
		case EXPR_OR: func = gen_expr_or; is_diff_type = true; break;
		case EXPR_XOR: func = gen_expr_xor; is_diff_type = true; break;
		case EXPR_IDENTIFIER:
			return gen_expr_identifier(type, expr->lhs.tok, err);
		case EXPR_CONST:
			return gen_expr_const(type, expr->lhs.tok, err);
		case EXPR_FUNC:
			return gen_expr_function_call(expr, type, err);
		default: return (Expr){0};
	}

	if (*err) return (Expr){0};

	const Token *lhs_token = expr->lhs.expr ?
		(expr->lhs.expr->func_call ?
			expr->lhs.expr->func_call->func_name_tok :
			expr->lhs.expr->lhs.tok) :
			NULL;

	const Expr lhs = lhs_token ?
		gen_expr(is_diff_type ? NULL : type, expr->lhs.expr, err) :
		(Expr){0};

	if (*err) return (Expr){0};

	const Token *rhs_token = expr->rhs.expr->lhs.tok;

	const Expr rhs = gen_expr(
		is_diff_type ? lhs.type : type,
		expr->rhs.expr,
		err
	);

	if (*err) return (Expr){0};

	if (lhs.value && lhs.type != rhs.type) {
		FMT_ERROR(ERR_EXPECTED_SAME_TYPE,
			{ .loc = &rhs_token->location, .type = lhs.type },
			{ .type = rhs.type }
		);

		*err = true;
		return (Expr){0};
	}

	const Expr result = func ?
			func(rhs.type, lhs.value, rhs.value) :
			func_with_err(rhs.type, lhs.value, rhs.value, err);

	if ((!result.type && !result.value) || *err) return (Expr){0};

	if (type && result.type != type) {
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
Convert `token` to an expression.

If `variable` is and `token` is a variable, store the found variable there.
*/
static Expr token_to_expr(
	const Token *const token,
	Variable **variable,
	bool *err
) {
	if (token->type == TOKEN_IDENTIFIER) {
		Variable *const var_found = scope_find_var(token, err);
		if (*err) return (Expr){0};

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
				SKULL_STATE.builder,
				gen_llvm_type(var_found->type),
				var_found->ref,
				""
			),
			.type = var_found->type
		};
	}

	return token_to_simple_expr(token, err);
}

/*
Make an expression from `token`, checking for compatibility with `type`.

Set `err` if an error occurred.
*/
static Expr gen_expr_const(
	Type type,
	const Token *const token,
	bool *err
) {
	const Expr expr = token_to_simple_expr(token, err);

	if (!expr.type && !*err) {
		FMT_ERROR(ERR_TYPE_MISMATCH, {
			.loc = &token->location,
			.type = type
		});

		*err = true;
		return (Expr){0};
	}

	return expr;
}

/*
Make a simple expression (const literal) from `token`.
*/
static Expr token_to_simple_expr(const Token *const token, bool *err) {
	LLVMValueRef value = NULL;
	Type type = NULL;

	if (token->type == TOKEN_INT_CONST) {
		value = LLVM_INT(eval_integer(token, err));
		type = TYPE_INT;
	}
	else if (token->type == TOKEN_FLOAT_CONST) {
		value = LLVM_FLOAT(eval_float(token, err));
		type = TYPE_FLOAT;
	}
	else if (token->type == TOKEN_BOOL_CONST) {
		value = LLVM_BOOL(eval_bool(token));
		type = TYPE_BOOL;
	}
	else if (token->type == TOKEN_RUNE_CONST) {
		value = LLVM_RUNE(eval_rune(token, err));
		type = TYPE_RUNE;
	}
	else if (token->type == TOKEN_STR_CONST) {
		char32_t *str = eval_str(token, err);
		if (!str) {
			*err = true;
			return (Expr){0};
		}

		char *const mbs = c32stombs(str, &token->location);
		if (!mbs) {
			free(str);
			*err = true;
			return (Expr){0};
		}

		value = LLVMBuildBitCast(
			SKULL_STATE.builder,
			LLVMBuildGlobalString(SKULL_STATE.builder, mbs, ""),
			gen_llvm_type(TYPE_STR),
			""
		);

		free(mbs);
		free(str);

		type = TYPE_STR;
	}

	return (Expr){
		.value = value,
		.type = type
	};
}
