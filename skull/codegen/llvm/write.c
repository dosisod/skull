#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <llvm-c/Analysis.h>
#include <llvm-c/Core.h>
#include <llvm-c/DebugInfo.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Linker.h>
#include <llvm-c/Target.h>
#include <llvm-c/Transforms/PassBuilder.h>

#include "skull/build_data.h"
#include "skull/codegen/abi.h"
#include "skull/codegen/llvm/debug.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/codegen/llvm/types.h"
#include "skull/codegen/llvm/write.h"
#include "skull/codegen/shared.h"
#include "skull/common/io.h"
#include "skull/common/str.h"
#include "skull/semantic/func.h"
#include "skull/semantic/symbol.h"
#include "skull/shim.h"

typedef enum {
	PHASE_DONE,
	PHASE_ERR,
	PHASE_NEXT,
} PhaseResult;

static bool setup_llvm_target_machine(SkullStateLLVM *);
static PhaseResult output_llvm_ir(const char *, const SkullStateLLVM *);
static PhaseResult verify_llvm(const SkullStateLLVM *);
static PhaseResult optimize_llvm(SkullStateLLVM *);
static PhaseResult emit_stage_1(const char *, SkullStateLLVM *);
static PhaseResult emit_native_binary(void);
static bool add_start_shim(SkullStateLLVM *);
static bool llvm_insert_builtins(SkullStateLLVM *);
static bool will_build_binary(void);
static int sh(char *[]);
static int shell(char *);
static char *get_binary_name(void);
static bool check_directory(char *);

/*
Writer for LLVM backend. Can emit LLVM, native binary, or assembler based
on CLI parameters.
*/
bool write_llvm(const char *filename, SkullStateLLVM *state) {
	if (BUILD_DATA.debug) LLVMDIBuilderFinalize(DEBUG_INFO.builder);

	// we need to add more LLVM boilerplate if we end up needing to actually
	// create a binary, though we still want it to be verified+optimized
	if (will_build_binary() && !add_start_shim(state)) return PHASE_ERR;

	PhaseResult result = verify_llvm(state);
	if (result != PHASE_NEXT) return result;

	result = optimize_llvm(state);
	if (result != PHASE_NEXT) return result;

	result = emit_stage_1(filename, state);
	if (result != PHASE_NEXT) return result;

	return emit_native_binary();
}

static PhaseResult verify_llvm(const SkullStateLLVM *state) {
	if (BUILD_DATA.llvm_no_verify) return PHASE_NEXT;

	char *err_msg = NULL;

	const bool err = LLVMVerifyModule(
		state->module,
		LLVMReturnStatusAction,
		&err_msg
	);

	if (err) {
		fprintf(stderr, "skull: error while verifying LLVM: %s\n", err_msg);
	}

	LLVMDisposeMessage(err_msg);

	return err ? PHASE_ERR : PHASE_NEXT;
}

static PhaseResult optimize_llvm(SkullStateLLVM *state) {
	if (!(
		BUILD_DATA.optimize1 ||
		BUILD_DATA.optimize2 ||
		BUILD_DATA.optimize3
	)) {
		return PHASE_NEXT;
	}

	if (setup_llvm_target_machine(state)) return PHASE_ERR;

	LLVMPassBuilderOptionsRef options = LLVMCreatePassBuilderOptions();

	const char *passes = BUILD_DATA.optimize1 ?
		"default<O1>" :
		BUILD_DATA.optimize2 ?
			"default<O2>" :
			"default<O3>";

	LLVMRunPasses(state->module, passes, state->target_machine, options);
	LLVMDisposePassBuilderOptions(options);

	return PHASE_NEXT;
}

static PhaseResult output_llvm_ir(
	const char *filename,
	const SkullStateLLVM *state
) {
	if (strcmp(filename, "-") == 0) {
		char *msg = LLVMPrintModuleToString(state->module);
		fprintf(stderr, "%s", msg);

		LLVMDisposeMessage(msg);
	}
	else {
		char *out_filename = get_new_filename(filename, "ll");

		char *msg = NULL;
		LLVMBool did_fail = LLVMPrintModuleToFile(
			state->module,
			out_filename,
			&msg
		);

		free(out_filename);

		if (did_fail) {
			fprintf(stderr, "skull: error occurred: %s\n", msg);

			LLVMDisposeMessage(msg);
			return PHASE_ERR;
		}
	}

	return PHASE_DONE;
}

static char *get_binary_name(void) {
	char *binary_name = strdup(BUILD_DATA.filename); // NOLINT
	char *last_dot = strrchr(binary_name, '.');
	binary_name[last_dot - binary_name] = '\0';

	return binary_name;
}

/*
Return true if an error occurs.
*/
static bool check_directory(char *binary_name) {
	if (is_directory(binary_name)) {
		fprintf(
			stderr,
			"skull: \"%s\" is a directory not a file\n",
			binary_name
		);
		free(binary_name);
		return true;
	}

	return false;
}

/*
Using previously compiled object file and C shim for built-in helpers, create
an executable binary.

Eventually this will use JIT compilation to compile the shim into the existing
LLVM module, but that is for later.
*/
static PhaseResult emit_native_binary(void) {
	char *binary_name = get_binary_name();
	bool err = check_directory(binary_name);

	if (err) return PHASE_ERR;

	char *shim = uvsnprintf(
		"cc -nostartfiles \"%s\" -o \"%s\" %s -I/usr/include/skull -lm",
		BUILD_DATA.out_file,
		binary_name,
		BUILD_DATA.extra_args ? BUILD_DATA.extra_args : (char[]){0}
	);
	free(binary_name);

	const bool exit_code = shell(shim);

	errno = 0;
	remove(BUILD_DATA.out_file);
	if (errno) perror("remove");

	free(shim);
	free(BUILD_DATA.extra_args);

	return exit_code ? PHASE_ERR : PHASE_DONE;
}

static int sh(char *argv[]) {
	errno = 0;
	const pid_t pid = fork();
	if (errno) {
		perror("fork");
		return 1;
	}

	if (pid == 0) {
		errno = 0;
		execvp(argv[0], argv);
		if (errno) {
			perror("execvp");
			return 1;
		}
	}

	int status = 0;
	wait(&status);

	return WEXITSTATUS(status);
}

static int shell(char *cmd) {
	return sh((char *[]){ (char[]){"/bin/sh"}, (char[]){"-c"}, cmd, NULL });
}

/*
Setup LLVM target machine. Return true if error occurs.
*/
static bool setup_llvm_target_machine(SkullStateLLVM *state) {
	if (state->target_machine) return false;

	LLVMInitializeAllTargetInfos();
	LLVMInitializeAllTargets();
	LLVMInitializeAllTargetMCs();
	LLVMInitializeAllAsmPrinters();
	LLVMInitializeAllAsmParsers();
	LLVMInitializeAllDisassemblers();

	char *triple = LLVMGetDefaultTargetTriple();
	LLVMTargetRef target = NULL;
	char *err_msg = NULL;

	const bool err = LLVMGetTargetFromTriple(triple, &target, &err_msg);

	if (err) {
		fprintf(stderr, "skull: \"%s\"\n", err_msg);
		LLVMDisposeMessage(err_msg);
		LLVMDisposeMessage(triple);

		return true;
	}

	char *cpu = LLVMGetHostCPUName();
	char *features = LLVMGetHostCPUFeatures();

	state->target_machine = LLVMCreateTargetMachine(
		target,
		triple,
		cpu,
		features,
		LLVMCodeGenLevelDefault,
		LLVMRelocDefault,
		LLVMCodeModelDefault
	);

	LLVMDisposeMessage(triple);
	LLVMDisposeMessage(cpu);
	LLVMDisposeMessage(features);

	if (!state->target_machine) {
		fprintf(stderr, "skull: target machine could not be created!\n");
		return true;
	}

	return false;
}

/*
Based on BUILD_DATA (cli flags), emit either:

* LLVM IR (to stdout or file)
* Object file
* Assembly file (to stdout or file)
*/
static PhaseResult emit_stage_1(const char *filename, SkullStateLLVM *state) {
	if (BUILD_DATA.preprocess) {
		return output_llvm_ir(filename, state);
	}

	if (setup_llvm_target_machine(state)) return PHASE_ERR;

	if (!BUILD_DATA.out_file) {
		BUILD_DATA.out_file = gen_filename(
			BUILD_DATA.filename,
			BUILD_DATA.asm_backend ? "s" : "o"
		);
	}

	if (!BUILD_DATA.asm_backend && strcmp(BUILD_DATA.out_file, "-") == 0) {
		fprintf(stderr, "skull: cannot print binary file to stdout\n");
		return PHASE_ERR;
	}

	char *err_msg = NULL;

	const bool err = LLVMTargetMachineEmitToFile(
		state->target_machine,
		state->module,
		BUILD_DATA.out_file,
		BUILD_DATA.asm_backend ? LLVMAssemblyFile : LLVMObjectFile,
		&err_msg
	);

	if (err || err_msg) {
		fprintf(stderr, "skull: \"%s\"\n", err_msg);
		return PHASE_ERR;
	}

	return (BUILD_DATA.compile_only || BUILD_DATA.asm_backend) ?
		PHASE_DONE :
		PHASE_NEXT;
}

static bool will_build_binary(void) {
	return !(
		BUILD_DATA.preprocess ||
		BUILD_DATA.compile_only ||
		BUILD_DATA.asm_backend
	);
}

/*
Add custom _start function (allows for using "main" as a function name).

Return `false` if an error occurred.
*/
static bool add_start_shim(SkullStateLLVM *state) {
	LLVMTypeRef start_type = type_to_llvm_func_type(
		&TYPE_VOID, NULL, 0, state
	);

	LLVMValueRef start_func = LLVMAddFunction(
		state->module,
		"_start",
		start_type
	);

	LLVMSetLinkage(start_func, LLVMExternalLinkage);
	LLVMAddTargetDependentFunctionAttr(start_func, "stackrealign", "");

	LLVMTypeRef exit_func_type = type_to_llvm_func_type(
		&TYPE_VOID,
		(LLVMTypeRef[]){ LLVMInt64TypeInContext(state->ctx) },
		1,
		state
	);

	LLVMValueRef exit_func = LLVMAddFunction(
		state->module,
		"exit",
		exit_func_type
	);

	LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(
		state->ctx,
		start_func,
		"entry"
	);

	LLVMPositionBuilderAtEnd(state->builder, entry);

	LLVMValueRef return_value = LLVMBuildCall2(
		state->builder,
		state->main_func->func->type,
		state->main_func->func->ref,
		NULL,
		0,
		""
	);

	LLVMBuildCall2(
		state->builder,
		exit_func_type,
		exit_func,
		(LLVMValueRef[]){ return_value },
		1,
		""
	);

	// we could add a noreturn attribute to the exit call, but this is easier
	// for now
	LLVMBuildRetVoid(state->builder);

	return llvm_insert_builtins(state);
}

/*
Add built-in functions (via LLVM IR), inserting them into the existing module
in `state`.

Return `false` if an error occurred.
*/
static bool llvm_insert_builtins(SkullStateLLVM *state) {
	LLVMMemoryBufferRef mem_buf = LLVMCreateMemoryBufferWithMemoryRangeCopy(
		LL_SHIM,
		LL_SHIM_len,
		"<jit>"
	);

	LLVMModuleRef shim_module = NULL;
	char *err_msg = NULL;

	bool failed = LLVMParseIRInContext(
		state->ctx,
		mem_buf,
		&shim_module,
		&err_msg
	);

	if (failed || err_msg) {
		fprintf(stderr, "%s\n", err_msg);
		LLVMDisposeErrorMessage(err_msg);
		return false;
	}

	failed = LLVMLinkModules2(state->module, shim_module);
	if (failed) {
		// there is a diagnostic handler you can setup to get the actual error,
		// but I couldn't figure out how to set it up correctly.
		fprintf(stderr, "skull: internal error\n");
		return false;
	}

	return true;
}
