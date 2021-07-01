# skull/codegen/llvm/aliases

```c
#define LLVM_INT(num) \
	LLVMConstInt( \
		LLVMInt64TypeInContext(SKULL_STATE_LLVM.ctx), \
		(unsigned long long)(num), \
		true \
	)
```

> Convert `num` to an `i64` LLVM value.

```c
#define LLVM_BOOL(val) \
	LLVMConstInt( \
		LLVMInt1TypeInContext(SKULL_STATE_LLVM.ctx), \
		val, \
		true \
	)
```

> Convert `val` to an `i1` LLVM value.

```c
#define LLVM_RUNE(c) \
	LLVMConstInt( \
		LLVMInt32TypeInContext(SKULL_STATE_LLVM.ctx), \
		c, \
		true \
	)
```

> Convert `c` to an `i32` (rune) LLVM value.

```c
#define LLVM_FLOAT(num) \
	LLVMConstReal( \
		LLVMDoubleTypeInContext(SKULL_STATE_LLVM.ctx), \
		num \
	)
```

> Convert `num` to a double as an LLVM value.

