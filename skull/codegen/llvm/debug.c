#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <llvm-c/DebugInfo.h>

#include "skull/build_data.h"
#include "skull/codegen/abi.h"
#include "skull/codegen/llvm/aliases.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/codegen/llvm/types.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/semantic/func.h"
#include "skull/semantic/symbol.h"
#include "skull/semantic/types.h"
#include "skull/semantic/variable.h"
#include "skull/version.h"

#include "skull/codegen/llvm/debug.h"


static LLVMMetadataRef type_to_llvm_di_type(const Type *);
static void alloc_debug_function_param(Variable *, const SkullStateLLVM *);
static void add_llvm_global_var_def_debug_info(const Symbol *);
static void add_llvm_local_var_def_debug_info(
	const Symbol *,
	const SkullStateLLVM *
);
static LLVMMetadataRef make_llvm_debug_location(
	const Location *,
	const SkullStateLLVM *
);

#define PRODUCER "clang (skull "SKULL_VERSION")"

DebugInfo DEBUG_INFO;
static LLVMMetadataRef DI_TYPE_BOOL;
static LLVMMetadataRef DI_TYPE_INT;
static LLVMMetadataRef DI_TYPE_FLOAT;
static LLVMMetadataRef DI_TYPE_RUNE;
static LLVMMetadataRef DI_TYPE_STR;

LLVMDIBuilderRef setup_debug_info(
	const char *filename,
	const SkullStateLLVM *state
) {
	LLVMDIBuilderRef di_builder = LLVMCreateDIBuilder(state->module);
	DEBUG_INFO.builder = di_builder;

	DI_TYPE_BOOL = type_to_llvm_di_type(&TYPE_BOOL);
	DI_TYPE_INT = type_to_llvm_di_type(&TYPE_INT);
	DI_TYPE_FLOAT = type_to_llvm_di_type(&TYPE_FLOAT);
	DI_TYPE_RUNE = type_to_llvm_di_type(&TYPE_RUNE);
	DI_TYPE_STR = type_to_llvm_di_type(&TYPE_STR);

	char cwd[PATH_MAX];

	errno = 0;
	if (!getcwd(cwd, PATH_MAX)) {
		perror("getcwd");
		return NULL;
	}

	LLVMMetadataRef di_file = LLVMDIBuilderCreateFile(
		di_builder,
		filename, strlen(filename),
		cwd, strlen(cwd)
	);
	DEBUG_INFO.file = di_file;

	DEBUG_INFO.compile_unit = LLVMDIBuilderCreateCompileUnit(
		di_builder,
		LLVMDWARFSourceLanguageC99,
		di_file,
		PRODUCER, strlen(PRODUCER),
		false, // is optimized
		"", 0, // flags
		0, // runtime version
		"", 0, // split name
		1, // debug kind
		0, // DWOId
		false, // emit inline debug info
		false, // debug info for profiling
		"", 0, // sysroot
		"", 0 // sdk
	);

	LLVMMetadataRef sub_type = LLVMDIBuilderCreateSubroutineType(
		di_builder,
		di_file,
		NULL, 0,
		LLVMDIFlagZero
	);

	// TODO(dosisod): this is already calculated when setting up main func,
	// figure out how to re-use it
	char *main_func_name = create_main_func_name(filename);

	DEBUG_INFO.scope = LLVMDIBuilderCreateFunction(
		di_builder,
		di_file,
		main_func_name, strlen(main_func_name),
		"", 0, // linkage name
		di_file,
		1, // line number
		sub_type,
		false, // is local to unit
		true, // is definition
		1, // scope line
		LLVMDIFlagZero,
		false // is optimized
	);
	free(main_func_name);

	LLVMSetSubprogram(state->main_func->func->ref, DEBUG_INFO.scope);

	LLVMAddModuleFlag(
		state->module,
		LLVMModuleFlagBehaviorOverride,
		"Dwarf Version", 13,
		LLVMValueAsMetadata(LLVM_INT32(4))
	);

	LLVMAddModuleFlag(
		state->module,
		LLVMModuleFlagBehaviorOverride,
		"Debug Info Version", 18,
		LLVMValueAsMetadata(LLVM_INT32(3))
	);

	return di_builder;
}

LLVMMetadataRef type_to_llvm_di_type(const Type *type) {
	if (type == &TYPE_STR) {
		return LLVMDIBuilderCreatePointerType(
			DEBUG_INFO.builder,
			LLVMDIBuilderCreateBasicType(
				DEBUG_INFO.builder,
				"char", 4,
				CHAR_BIT,
				DW_ATE_signed_char,
				LLVMDIFlagZero
			),
			64, // size in bits
			0, // align in bits
			0, // address space
			type->name, strlen(type->name)
		);
	}
	if (is_reference(type)) {
		LLVMMetadataRef md = LLVMDIBuilderCreatePointerType(
			DEBUG_INFO.builder,
			type_to_llvm_di_type(type->inner),
			64,
			0,
			0,
			type->name, strlen(type->name)
		);

		return md;
	}

	unsigned bits = 0;
	DW_ATE encoding = DW_ATE_signed;

	if (type == &TYPE_BOOL) {
		bits = 1;
		encoding = DW_ATE_boolean;
	}
	else if (type == &TYPE_INT) {
		bits = 64;
	}
	else if (type == &TYPE_FLOAT) {
		bits = sizeof(double) * CHAR_BIT;
		encoding = DW_ATE_float;
	}
	else if (type == &TYPE_RUNE) {
		bits = 32;
		encoding = DW_ATE_UTF;
	}

	return LLVMDIBuilderCreateBasicType(
		DEBUG_INFO.builder,
		type->name, strlen(type->name),
		bits,
		encoding,
		LLVMDIFlagZero
	);
}

void add_llvm_debug_info(
	LLVMValueRef value,
	const Location *location,
	const SkullStateLLVM *state
) {
	if (BUILD_DATA.debug && !LLVMIsConstant(value)) {
		LLVMInstructionSetDebugLoc(
			value,
			make_llvm_debug_location(location, state)
		);
	}
}

static LLVMMetadataRef make_llvm_debug_location(
	const Location *location,
	const SkullStateLLVM *state
) {
	return LLVMDIBuilderCreateDebugLocation(
		state->ctx,
		location->line,
		location->column,
		DEBUG_INFO.scope,
		NULL
	);
}

LLVMMetadataRef __attribute__((pure)) type_to_di_type(const Type *type) {
	if (type == &TYPE_BOOL) return DI_TYPE_BOOL;
	if (type == &TYPE_INT) return DI_TYPE_INT;
	if (type == &TYPE_FLOAT) return DI_TYPE_FLOAT;
	if (type == &TYPE_RUNE) return DI_TYPE_RUNE;
	if (type == &TYPE_STR) return DI_TYPE_STR;

	if (type->inner) {
		return type_to_llvm_di_type(type);
	}

	return NULL;
}

void add_llvm_var_def_debug_info(
	const Symbol *symbol,
	const SkullStateLLVM *state
) {
	if (!BUILD_DATA.debug) return;

	if (symbol->var->is_global) add_llvm_global_var_def_debug_info(symbol);
	else add_llvm_local_var_def_debug_info(symbol, state);
}

static void add_llvm_global_var_def_debug_info(const Symbol *symbol) {
	LLVMMetadataRef metadata = LLVMDIBuilderCreateGlobalVariableExpression(
		DEBUG_INFO.builder,
		DEBUG_INFO.compile_unit,
		symbol->var->name, strlen(symbol->var->name),
		"", 0, // linkage
		DEBUG_INFO.file,
		symbol->location.line,
		type_to_di_type(symbol->var->type),
		!symbol->var->is_exported, // local to unit
		LLVMDIBuilderCreateExpression(DEBUG_INFO.builder, NULL, 0),
		NULL, // decl
		8 // alignment
	);

	LLVMGlobalSetMetadata(symbol->var->ref, 0, metadata);
}

static void add_llvm_local_var_def_debug_info(
	const Symbol *symbol,
	const SkullStateLLVM *state
) {
	LLVMMetadataRef di_var = LLVMDIBuilderCreateAutoVariable(
		DEBUG_INFO.builder,
		DEBUG_INFO.scope,
		symbol->var->name, strlen(symbol->var->name),
		DEBUG_INFO.file,
		symbol->location.line,
		type_to_di_type(symbol->var->type),
		true, // survive optimizations
		LLVMDIFlagZero,
		8 // alignment
	);

	LLVMDIBuilderInsertDeclareAtEnd(
		DEBUG_INFO.builder,
		symbol->var->ref,
		di_var,
		LLVMDIBuilderCreateExpression(DEBUG_INFO.builder, NULL, 0),
		make_llvm_debug_location(&symbol->location, state),
		LLVMGetInsertBlock(state->builder)
	);
}

LLVMMetadataRef add_llvm_control_flow_debug_info(const Location *location) {
	if (!BUILD_DATA.debug) return NULL;

	LLVMMetadataRef old_di_scope = DEBUG_INFO.scope;

	DEBUG_INFO.scope = LLVMDIBuilderCreateLexicalBlock(
		DEBUG_INFO.builder,
		DEBUG_INFO.scope,
		DEBUG_INFO.file,
		location->line,
		location->column
	);

	return old_di_scope;
}

LLVMMetadataRef add_llvm_func_debug_info(
	Symbol *symbol,
	const SkullStateLLVM *state
) {
	if (!BUILD_DATA.debug) return NULL;

	FunctionDeclaration *func = symbol->func;

	LLVMMetadataRef *di_param_types = Malloc(
		(func->num_params + 1) * sizeof(LLVMMetadataRef)
	);
	di_param_types[0] = type_to_di_type(func->return_type);

	if (func->num_params) {
		for (unsigned i = 1; i <= func->num_params; i++) {
			di_param_types[i] = type_to_di_type(func->param_types[i - 1]);
		}
	}

	LLVMMetadataRef di_type = LLVMDIBuilderCreateSubroutineType(
		DEBUG_INFO.builder,
		DEBUG_INFO.file,
		di_param_types, func->num_params + 1,
		LLVMDIFlagZero
	);
	free(di_param_types);

	LLVMMetadataRef new_di_scope = LLVMDIBuilderCreateFunction(
		DEBUG_INFO.builder,
		DEBUG_INFO.file,
		symbol->name, strlen(symbol->name),
		"", 0,
		DEBUG_INFO.file,
		symbol->location.line,
		di_type,
		false,
		!func->is_external,
		symbol->location.line,
		LLVMDIFlagZero,
		false
	);
	LLVMMetadataRef old_di_scope = DEBUG_INFO.scope;
	DEBUG_INFO.scope = new_di_scope;
	LLVMSetSubprogram(func->ref, DEBUG_INFO.scope);

	if (func->num_params) {
		for (unsigned i = 0; i < func->num_params; i++) {
			char *param_name = c32stombs(
				func->params[i]->param_name,
				&symbol->location
			);

			LLVMMetadataRef di_var = LLVMDIBuilderCreateParameterVariable(
				DEBUG_INFO.builder,
				DEBUG_INFO.scope,
				param_name, strlen(param_name),
				i + 1,
				DEBUG_INFO.file,
				symbol->location.line,
				type_to_di_type(func->param_types[i]),
				true,
				LLVMDIFlagZero
			);
			free(param_name);

			alloc_debug_function_param(func->params[i]->symbol->var, state);

			LLVMDIBuilderInsertDeclareAtEnd(
				DEBUG_INFO.builder,
				func->params[i]->symbol->var->ref,
				di_var,
				LLVMDIBuilderCreateExpression(DEBUG_INFO.builder, NULL, 0),
				make_llvm_debug_location(&symbol->location, state),
				LLVMGetEntryBasicBlock(func->ref)
			);
		}
	}

	return old_di_scope;
}

static void alloc_debug_function_param(
	Variable *var,
	const SkullStateLLVM *state
) {
	LLVMValueRef old_ref = var->ref;
	Variable *func_var = var;

	func_var->ref = LLVMBuildAlloca(
		state->builder,
		type_to_llvm_type(func_var->type, state),
		func_var->name
	);

	LLVMBuildStore(state->builder, old_ref, func_var->ref);

	var->ref = func_var->ref;
	var->is_const = false;
}
