#pragma once

#include <llvm-c/Core.h>

#include "skull/codegen/var.h"

Expr gen_expr_oper(
	Type,
	const AstNodeExpr *const,
	bool *
);
