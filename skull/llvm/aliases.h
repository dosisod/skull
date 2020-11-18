/*
Convert `num` to an `i64` LLVM value.
*/
#define LLVM_INT(num) \
	LLVMConstInt( \
		LLVMInt64Type(), \
		(unsigned long long)(num), \
		true \
	)

/*
Convert `val` to an `i1` LLVM value.
*/
#define LLVM_BOOL(val) \
	LLVMConstInt( \
		LLVMInt1Type(), \
		val, \
		true \
	)

/*
Convert `c` to an `i32` (rune) LLVM value.
*/
#define LLVM_RUNE(c) \
	LLVMConstInt( \
		LLVMInt32Type(), \
		c, \
		true \
	)

/*
Convert `num` to a double as an LLVM value.
*/
#define LLVM_FLOAT(num) \
	LLVMConstReal( \
		LLVMDoubleType(), \
		num \
	)

