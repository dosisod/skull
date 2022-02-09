#pragma once

/*
Convert `num` to an `i64` LLVM value.
*/
#define LLVM_INT(num) \
	LLVMConstInt( \
		LLVMInt64TypeInContext(state->ctx), \
		(unsigned long long)(num), \
		1 \
	)

/*
Convert `num` to an `i32` LLVM value.
*/
#define LLVM_INT32(num) \
	LLVMConstInt( \
		LLVMInt32TypeInContext(state->ctx), \
		(unsigned long long)(num), \
		1 \
	)

/*
Convert `val` to an `i1` LLVM value.
*/
#define LLVM_BOOL(val) \
	LLVMConstInt( \
		LLVMInt1TypeInContext(state->ctx), \
		val, \
		1 \
	)

/*
Convert `c` to an `i32` (rune) LLVM value.
*/
#define LLVM_RUNE(c) LLVM_INT32(c)

/*
Convert `num` to a double as an LLVM value.
*/
#define LLVM_FLOAT(num) \
	LLVMConstReal( \
		LLVMDoubleTypeInContext(state->ctx), \
		num \
	)

