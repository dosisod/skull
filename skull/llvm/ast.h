#include <llvm-c/Core.h>

#include "skull/eval/context.h"
#include "skull/parse/ast/node.h"

void str_to_llvm_ir(char *, LLVMValueRef, LLVMBuilderRef, LLVMContextRef);

void llvm_make_return(AstNode **, Context *, LLVMContextRef, LLVMBuilderRef);
void llvm_make_var_def(AstNode **, Context *);
void llvm_make_if(AstNode **, LLVMValueRef, LLVMContextRef, LLVMBuilderRef);
