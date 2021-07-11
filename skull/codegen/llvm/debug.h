#pragma once

#include "skull/codegen/llvm/fwd_decl.h"

typedef struct Location Location;
typedef const char *Type;

typedef struct {
	LLVMDIBuilderRef builder;
	LLVMMetadataRef file;
	LLVMMetadataRef scope;
} DebugInfo;

extern DebugInfo DEBUG_INFO;

extern LLVMMetadataRef DI_TYPE_BOOL;
extern LLVMMetadataRef DI_TYPE_INT;
extern LLVMMetadataRef DI_TYPE_FLOAT;
extern LLVMMetadataRef DI_TYPE_RUNE;
extern LLVMMetadataRef DI_TYPE_STR;

LLVMDIBuilderRef setup_debug_info(
	const char *,
	LLVMModuleRef
);

void add_llvm_debug_info(LLVMValueRef, Location *);

LLVMMetadataRef type_to_di_type(Type);
