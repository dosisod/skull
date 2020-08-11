#define LLVM_INT(ctx, num) \
	LLVMConstInt( \
		LLVMInt64TypeInContext(ctx), \
		(unsigned long long)num, \
		true \
	)

