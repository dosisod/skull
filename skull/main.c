#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/color.h"
#include "skull/common/io.h"
#include "skull/common/str.h"
#include "skull/llvm/ast.h"

#ifndef SKULL_VERSION
#define SKULL_VERSION "<version unknown>"
#endif

#define DIE(x) puts(x); return 1

int main(int argc, char *argv[]) {
	if (argc == 1) {
		puts("usage: skull [-v | file [ -- args ]]");
		return 0;
	}

	if (argc > 2) {
		DIE("too many arguments passed, exiting");
	}

	if (strcmp("-v", argv[1]) == 0) {
		puts(COLOR_GREEN_FG "Skull " COLOR_RESET SKULL_VERSION);
		return 0;
	}

	if (!strrstr(argv[1], ".sk")) {
		DIE("missing required \".sk\" extension, exiting");
	}

	if (strrstr(argv[1], ".sk") == argv[1] || strrstr(argv[1], "/.sk")) {
		DIE("\".sk\" is not a valid name, exiting");
	}

	errno = 0;
	FILE *const f = fopen(argv[1], "re");
	if (!f) {
		if (errno == EACCES) {
			printf("cannot open \"%s\", permission denied\n", argv[1]);
		}
		else if (errno == ENOENT) {
			printf("\"%s\" was not found, exiting\n", argv[1]);
		}
		return 1;
	}

	LLVMModuleRef main_module = LLVMModuleCreateWithName(argv[1]);

	LLVMTypeRef main_func_type = LLVMFunctionType(
		LLVMInt64Type(),
		NULL,
		0,
		false
	);

	LLVMValueRef main_func = LLVMAddFunction(
		main_module,
		"main",
		main_func_type
	);

	LLVMBasicBlockRef entry = LLVMAppendBasicBlock(
		main_func,
		"entry"
	);

	LLVMBuilderRef builder = LLVMCreateBuilder();

	LLVMPositionBuilderAtEnd(
		builder,
		entry
	);

	char *const file_contents = read_file(f);

	str_to_llvm_ir(
		file_contents,
		main_func,
		builder,
		main_module
	);

	free(file_contents);

	const size_t len = strlen(argv[1]);
	char *const ll_filename = malloc(len + 5);

	const char *const slash_pos = strrchr(argv[1], '/');
	if (!slash_pos) {
		ll_filename[0] = '.';
		memcpy(ll_filename + 1, argv[1], len);
	}
	else {
		const long offset = slash_pos - argv[1];

		memcpy(ll_filename, argv[1], len);
		ll_filename[offset + 1] = '.';
		memcpy(ll_filename + offset + 2, slash_pos + 1, len - (size_t)offset);
	}
	memcpy(ll_filename + len + 1, ".ll", 4);

	char *err = NULL;
	LLVMBool status = LLVMPrintModuleToFile(
		main_module,
		ll_filename,
		&err
	);
	free(ll_filename);

	if (err || status) {
		printf("error occurred: %s\n", err);
		LLVMDisposeMessage(err);
		return 1;
	}

	LLVMDisposeMessage(err);
	return 0;
}