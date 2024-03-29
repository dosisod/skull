#pragma once

#include "skull/codegen/llvm/fwd_decl.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/semantic/types.h"

typedef struct Location Location;
typedef struct Variable Variable;
typedef struct FunctionDeclaration FunctionDeclaration;

typedef struct {
	LLVMDIBuilderRef builder;
	LLVMMetadataRef file;
	LLVMMetadataRef scope;
	LLVMMetadataRef compile_unit;
} DebugInfo;

typedef enum {
	DW_ATE_address = 1,
	DW_ATE_boolean = 2,
	DW_ATE_complex_float = 3,
	DW_ATE_float = 4,
	DW_ATE_signed = 5,
	DW_ATE_signed_char = 6,
	DW_ATE_unsigned = 7,
	DW_ATE_unsigned_char = 8,
	DW_ATE_imaginary_float = 9,
	DW_ATE_packed_decimal = 10,
	DW_ATE_numeric_string = 11,
	DW_ATE_edited = 12,
	DW_ATE_signed_fixed = 13,
	DW_ATE_unsigned_fixed = 14,
	DW_ATE_decimal_float = 15,
	DW_ATE_UTF = 16,
	DW_ATE_UCS = 17,
	DW_ATE_ASCII = 18
} DW_ATE;

extern DebugInfo DEBUG_INFO;

LLVMDIBuilderRef setup_debug_info(
	const char *,
	const SkullStateLLVM *
);

void add_llvm_debug_info(
	LLVMValueRef,
	const Location *,
	const SkullStateLLVM *
);

LLVMMetadataRef type_to_di_type(const Type *);
void add_llvm_var_def_debug_info(const Symbol *, const SkullStateLLVM *);
LLVMMetadataRef add_llvm_control_flow_debug_info(const Location *);
LLVMMetadataRef add_llvm_func_debug_info(
	Symbol *,
	const SkullStateLLVM *
);
