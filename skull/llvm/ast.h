#include <llvm-c/Core.h>

#include "skull/eval/scope.h"
#include "skull/parse/ast/node.h"

void str_to_llvm_ir(char *, LLVMValueRef, LLVMBuilderRef, LLVMContextRef);

void llvm_make_return(AstNode **, Scope *, LLVMContextRef, LLVMBuilderRef);
void llvm_make_var_def(AstNode **, Scope *);
void llvm_make_if(AstNode **, LLVMValueRef, LLVMContextRef, LLVMBuilderRef);
