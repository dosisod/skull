#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "skull/build_data.h"
#include "skull/cc_shim.h"
#include "skull/codegen/abi.h"
#include "skull/codegen/llvm/cc.h"
#include "skull/common/io.h"
#include "skull/common/str.h"
#include "skull/real_main.h"

static int run_llc(void);
static int run_cc(char *);
static int sh(char *[]);
static int shell(char *);
static char *get_llc_binary(void);

int handle_file(void) {
	char *binary_name = strdup(BUILD_DATA.filename); // NOLINT
	int err = real_main(2, (char *[]){ NULL, binary_name, NULL });

	if (err || BUILD_DATA.preprocess || BUILD_DATA.c_backend) {
		free(binary_name);
		return err;
	}

	err = run_llc();
	if (err) {
		free(binary_name);
		return err;
	}

	if (BUILD_DATA.asm_backend || BUILD_DATA.compile_only) {
		free(binary_name);
		return 0;
	}

	char *last_dot = strrchr(binary_name, '.');
	binary_name[last_dot - binary_name] = '\0';

	if (is_directory(binary_name)) {
		printf("skull: \"%s\" is a directory not a file\n", binary_name);
		free(binary_name);
		return 1;
	}

	return run_cc(binary_name);
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
		puts("skull: llc-10 command not found");
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
	const bool cmd_exists = !shell((char[]){"which "_cmd" > /dev/null"}); \
	static char cmd[] = _cmd; \
	if (cmd_exists) return cmd; \
}

static char *get_llc_binary(void) {
	CHECK_CMD("llc-10")
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
