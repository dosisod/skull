#include <llvm-c/Core.h>

#include "skull/eval/variable.h"

void var_to_llvm_ir(variable_t *, LLVMBuilderRef, LLVMContextRef);
