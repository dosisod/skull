#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <llvm-c/DebugInfo.h>

#include "skull/codegen/llvm/aliases.h"
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

	char cwd[256];
	errno = 0;
	getcwd(cwd, 256);

	if (errno) {
		perror("getcwd");
		return NULL;
	}

	LLVMMetadataRef di_file = LLVMDIBuilderCreateFile(
		di_builder,
		filename, strlen(filename),
		cwd, strlen(cwd)
	);

	LLVMDIBuilderCreateCompileUnit(
		di_builder,
		LLVMDWARFSourceLanguageC,
		di_file,
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

	LLVMMetadataRef sub_type = LLVMDIBuilderCreateSubroutineType(
		di_builder,
		di_file,
		&DI_TYPE_INT,
		1,
		LLVMDIFlagZero
	);

	// TODO(dosisod): this is already calculated when setting up main func,
	// figure out how to re-use it
	char *main_func_name = create_main_func_name(filename);

	DEBUG_INFO.scope = LLVMDIBuilderCreateFunction(
		di_builder,
		di_file,
		main_func_name, strlen(main_func_name),
		"", 0,
		di_file,
		1,
		sub_type,
		false,
		true,
		1,
		LLVMDIFlagZero,
		false
	);
	free(main_func_name);

	LLVMSetSubprogram(SKULL_STATE_LLVM.main_func->ref, DEBUG_INFO.scope);

	LLVMDIBuilderFinalize(di_builder);

	LLVMAddModuleFlag(
		module,
		LLVMModuleFlagBehaviorOverride,
		"Dwarf Version", 13,
		LLVMValueAsMetadata(LLVM_INT32(4))
	);

	LLVMAddModuleFlag(
		module,
		LLVMModuleFlagBehaviorOverride,
		"Debug Info Version", 18,
		LLVMValueAsMetadata(LLVM_INT32(3))
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

