# skull/llvm/aliases

```c
#define LLVM_INT(num) \
	LLVMConstInt( \
		LLVMInt64Type(), \
		(unsigned long long)(num), \
		true \
	)
```

> Convert `num` to an `i64` LLVM value.

```c
#define LLVM_BOOL(val) \
	LLVMConstInt( \
		LLVMInt1Type(), \
		val, \
		true \
	)
```

> Convert `val` to an `i1` LLVM value.

```c
#define LLVM_RUNE(c) \
	LLVMConstInt( \
		LLVMInt32Type(), \
		c, \
		true \
	)
```

> Convert `c` to an `i32` (rune) LLVM value.

```c
#define LLVM_FLOAT(num) \
	LLVMConstReal( \
		LLVMDoubleType(), \
		num \
	)
```

> Convert `num` to a double as an LLVM value.

