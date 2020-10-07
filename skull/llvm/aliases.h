#define LLVM_INT(num) \
	LLVMConstInt( \
		LLVMInt64Type(), \
		(unsigned long long)(num), \
		true \
	)

#define LLVM_BOOL(val) \
	LLVMConstInt( \
		LLVMInt1Type(), \
		val, \
		true \
	)

#define LLVM_RUNE(c) \
	LLVMConstInt( \
		LLVMInt32Type(), \
		c, \
		true \
	)

#define LLVM_FLOAT(num) \
	LLVMConstReal( \
		LLVMDoubleType(), \
		num \
	)
