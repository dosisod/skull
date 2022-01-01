#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <llvm-c/Core.h>
#include <llvm-c/DebugInfo.h>
#include <llvm-c/Target.h>
#include <llvm-c/Transforms/PassBuilder.h>

#include "skull/build_data.h"
#include "skull/cc_shim.h"
#include "skull/codegen/abi.h"
#include "skull/codegen/llvm/debug.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/codegen/llvm/write.h"
#include "skull/common/io.h"
#include "skull/common/str.h"

static bool create_ll_file(const char *);
static bool optimize_llvm(void);
static int run_llc(void);
static int run_cc(char *);
static int sh(char *[]);
static int shell(char *);
static char *get_llc_binary(void);
static char *get_binary_name(void);
static int check_directory(char *);

/*
Write LLVM code to `filename`, return whether error occured.
*/
bool write_file_llvm(const char *filename) {
	if (BUILD_DATA.optimize1 || BUILD_DATA.optimize2 || BUILD_DATA.optimize3) {
		if (optimize_llvm()) return true;
	}

	bool err = create_ll_file(filename);

	if (err || BUILD_DATA.preprocess) return err;

	err = run_llc();
	if (err || BUILD_DATA.asm_backend || BUILD_DATA.compile_only) return err;

	char *binary_name = get_binary_name();
	err = check_directory(binary_name);

	return err || run_cc(binary_name);
}

static bool optimize_llvm(void) {
	LLVMInitializeAllTargetInfos();

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

	LLVMTargetMachineRef target_machine = LLVMCreateTargetMachine(
		target,
		triple,
		cpu,
		features,
		LLVMCodeGenLevelDefault,
		LLVMRelocDefault,
		LLVMCodeModelDefault
	);

	LLVMPassBuilderOptionsRef options = LLVMCreatePassBuilderOptions();

	const char *passes = BUILD_DATA.optimize1 ?
		"default<O1>" :
		BUILD_DATA.optimize2 ?
			"default<O2>" :
			"default<O3>";

	LLVMRunPasses(SKULL_STATE_LLVM.module, passes, target_machine, options);

	LLVMDisposeMessage(triple);
	LLVMDisposeMessage(cpu);
	LLVMDisposeMessage(features);
	LLVMDisposeTargetMachine(target_machine);
	LLVMDisposePassBuilderOptions(options);

	return false;
}

static bool create_ll_file(const char *filename) {
	if (BUILD_DATA.debug) LLVMDIBuilderFinalize(DEBUG_INFO.builder);

	if (strcmp(filename, "-") == 0) {
		char *msg = LLVMPrintModuleToString(SKULL_STATE_LLVM.module);
		printf("%s", msg);

		LLVMDisposeMessage(msg);
		return false;
	}

	char *msg = NULL;
	LLVMBool did_fail = LLVMPrintModuleToFile(
		SKULL_STATE_LLVM.module,
		filename,
		&msg
	);

	if (did_fail) fprintf(stderr, "skull: error occurred: %s\n", msg);

	LLVMDisposeMessage(msg);
	return did_fail;
}

static char *get_binary_name(void) {
	char *binary_name = strdup(BUILD_DATA.filename); // NOLINT
	char *last_dot = strrchr(binary_name, '.');
	binary_name[last_dot - binary_name] = '\0';

	return binary_name;
}

static int check_directory(char *binary_name) {
	if (is_directory(binary_name)) {
		printf("skull: \"%s\" is a directory not a file\n", binary_name);
		free(binary_name);
		return 1;
	}

	return 0;
}

static int run_llc(void) {
	if (!BUILD_DATA.out_file) {
		BUILD_DATA.out_file = gen_filename(
			BUILD_DATA.filename,
			BUILD_DATA.asm_backend ? "s" : "o"
		);
	}

	if (!BUILD_DATA.asm_backend && strcmp(BUILD_DATA.out_file, "-") == 0) {
		puts("skull: cannot print binary file to stdout");
		return 1;
	}

	char *llc_cmd = get_llc_binary();
	if (!llc_cmd) {
		puts("skull: llc command not found");
		return 1;
	}

	char *llvm_file = gen_filename(BUILD_DATA.filename, "ll");

	char *args[] = {
		llc_cmd,
		llvm_file,
		BUILD_DATA.asm_backend ?
			(char[]){"-filetype=asm"} :
			(char[]){"-filetype=obj"},
		(char[]){"-o"},
		BUILD_DATA.out_file,
		NULL
	};

	const int exit_code = sh(args);

	errno = 0;
	remove(llvm_file);

	free(llvm_file);

	if (errno) {
		perror("remove");
		return 1;
	}

	return exit_code;
}

#define CHECK_CMD(_cmd) { \
	const bool cmd_exists = !shell((char[]){"which "_cmd" > /dev/null 2>&1"}); \
	static char cmd[] = _cmd; \
	if (cmd_exists) return cmd; \
}

static char *get_llc_binary(void) {
	// LLVM 13 is the recommend version, with no prefix, we assume it is version
	// 13 (should check this in the future), otherwise, check if version 12
	// exists (should still work, so just check it last).

	CHECK_CMD("llc-13")
	CHECK_CMD("llc")

	return NULL;
}
#undef CHECK_CMD

static int run_cc(char *binary_name) {
	char *module_name = create_main_func_name(BUILD_DATA.filename);

	char *shim = uvsnprintf(
		SHIM,
		module_name,
		BUILD_DATA.out_file,
		binary_name,
		BUILD_DATA.extra_args ? BUILD_DATA.extra_args : (char[]){0}
	);
	free(module_name);
	free(binary_name);

	const int exit_code = shell(shim);

	remove(BUILD_DATA.out_file);
	free(shim);
	free(BUILD_DATA.extra_args);

	return exit_code;
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
