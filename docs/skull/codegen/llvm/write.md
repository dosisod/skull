# skull/codegen/llvm/write

```c
bool write_llvm(const char *filename, SkullStateLLVM *state)
```

> Writer for LLVM backend. Can emit LLVM, native binary, or assembler based
> on CLI parameters.

```c
static bool check_directory(char *binary_name)
```

> Return true if an error occurs.

```c
static PhaseResult emit_native_binary(void)
```

> Using previously compiled object file and C shim for built-in helpers, create
> an executable binary.
> \
> Eventually this will use JIT compilation to compile the shim into the existing
> LLVM module, but that is for later.

```c
static bool setup_llvm_target_machine(SkullStateLLVM *state)
```

> Setup LLVM target machine. Return true if error occurs.

```c
static PhaseResult emit_stage_1(const char *filename, SkullStateLLVM *state)
```

> Based on BUILD_DATA (cli flags), emit either:
> \
> * LLVM IR (to stdout or file)
> * Object file
> * Assembly file (to stdout or file)

```c
static bool add_start_shim(SkullStateLLVM *state)
```

> Add custom _start function (allows for using "main" as a function name).
> \
> Return `false` if an error occurred.

```c
static bool llvm_insert_builtins(SkullStateLLVM *state)
```

> Add built-in functions (via LLVM IR), inserting them into the existing module
> in `state`.
> \
> Return `false` if an error occurred.

