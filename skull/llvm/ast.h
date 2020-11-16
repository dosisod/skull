#pragma once

#include <llvm-c/Core.h>

#include "skull/parse/ast/node.h"

void str_to_llvm_ir(char *const, LLVMValueRef, LLVMModuleRef, LLVMBuilderRef);
_Bool node_to_llvm_ir(AstNode *);
