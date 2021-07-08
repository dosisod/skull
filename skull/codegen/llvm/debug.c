#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <llvm-c/DebugInfo.h>

#include "skull/codegen/llvm/shared.h"
#include "skull/common/malloc.h"
#include "skull/semantic/func.h"
#include "skull/semantic/types.h"

#include "skull/codegen/llvm/debug.h"


static LLVMMetadataRef gen_llvm_di_type(const Type);

DebugInfo DEBUG_INFO;
LLVMMetadataRef DI_TYPE_BOOL;
LLVMMetadataRef DI_TYPE_INT;
LLVMMetadataRef DI_TYPE_FLOAT;
LLVMMetadataRef DI_TYPE_RUNE;
LLVMMetadataRef DI_TYPE_STR;

LLVMDIBuilderRef setup_debug_info(
	const char *filename,
	LLVMModuleRef module
) {
	LLVMDIBuilderRef di_builder = LLVMCreateDIBuilder(module);
	DEBUG_INFO.builder = di_builder;

	DI_TYPE_BOOL = gen_llvm_di_type(TYPE_BOOL);
	DI_TYPE_INT = gen_llvm_di_type(TYPE_INT);
	DI_TYPE_FLOAT = gen_llvm_di_type(TYPE_FLOAT);
	DI_TYPE_RUNE = gen_llvm_di_type(TYPE_RUNE);
	DI_TYPE_STR = gen_llvm_di_type(TYPE_STR);

	LLVMMetadataRef file = LLVMDIBuilderCreateFile(
		di_builder,
		filename, strlen(filename),
		".", 1
	);

	LLVMDIBuilderCreateCompileUnit(
		di_builder,
		LLVMDWARFSourceLanguageC,
		file,
		"skull", 5,
		false,
		"", 0,
		0,
		"", 0,
		1,
		0,
		false,
		false
	);

	LLVMMetadataRef type = LLVMDIBuilderCreateBasicType(
		di_builder,
		"main_func_type", 14,
		sizeof(void *),
		0, // DWARF encoding
		LLVMDIFlagZero
	);

	LLVMMetadataRef sub_type = LLVMDIBuilderCreateSubroutineType(
		di_builder,
		file,
		&type,
		1,
		LLVMDIFlagZero
	);

	DEBUG_INFO.scope = LLVMDIBuilderCreateFunction(
		di_builder,
		file,
		"main", 4,
		"", 0,
		file,
		1,
		sub_type,
		false,
		true,
		1,
		LLVMDIFlagZero,
		false
	);

	LLVMSetSubprogram(SKULL_STATE_LLVM.main_func->ref, DEBUG_INFO.scope);

	LLVMDIBuilderFinalize(di_builder);

	LLVMAddModuleFlag(
		module,
		LLVMModuleFlagBehaviorOverride,
		"Dwarf Version", 13,
		LLVMValueAsMetadata(LLVMConstInt(
			LLVMInt32Type(),
			(unsigned long long)(4),
			true
		))
	);

	LLVMAddModuleFlag(
		module,
		LLVMModuleFlagBehaviorOverride,
		"Debug Info Version", 18,
		LLVMValueAsMetadata(LLVMConstInt(
			LLVMInt32Type(),
			(unsigned long long)(3),
			true
		))
	);

	return di_builder;
}

LLVMMetadataRef gen_llvm_di_type(const Type type) {
	unsigned size = 0;

	if (type == TYPE_BOOL) {
		size = 1;
	}
	else if (type == TYPE_INT) {
		size = 64;
	}
	else if (type == TYPE_FLOAT) {
		size = sizeof(double);
	}
	else if (type == TYPE_RUNE) {
		size = 32;
	}
	else if (type == TYPE_STR) {
		size = sizeof(char *);
	}

	return LLVMDIBuilderCreateBasicType(
		DEBUG_INFO.builder,
		type,
		strlen(type),
		size,
		0,
		LLVMDIFlagZero
	);
}

