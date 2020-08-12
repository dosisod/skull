#define LLVM_INT(ctx, num) \
	LLVMConstInt( \
		LLVMInt64TypeInContext(ctx), \
		(unsigned long long)num, \
		true \
	)

#define LLVM_BOOL(ctx, val) \
	LLVMConstInt( \
		LLVMInt1TypeInContext(ctx), \
		val, \
		true \
	)

