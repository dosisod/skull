#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/color.h"
#include "skull/common/errors.h"
#include "skull/common/io.h"
#include "skull/common/local.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/llvm/ast.h"
#include "skull/setup_main.h"

#define DIE(x) puts(x); return 1

int setup_main(int argc, char *argv[]) {
	SETUP_LOCALE();

	if (1 == argc || argc > 3) {
		DIE("unexpected number of parameters");
	}

	const char *main_func_name = (argc == 3 && *argv[2]) ?
		argv[2] :
		"main";

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

	char *const file_contents = read_file(f);
	if (!file_contents) {
		PANIC(ERR_FILE_EMPTY, {0});
	}
	fclose(f);

	LLVMModuleRef main_module = generate_llvm(argv[1], main_func_name, file_contents);
	free(file_contents);

	char *llvm_filename = create_llvm_filename(argv[1]);

	char *err = NULL;
	LLVMBool status = LLVMPrintModuleToFile(
		main_module,
		llvm_filename,
		&err
	);

	LLVMDisposeModule(main_module);
	LLVMContextDispose(LLVMGetGlobalContext());
	free(llvm_filename);

	if (err || status) {
		printf("error occurred: %s\n", err);
		LLVMDisposeMessage(err);
		return 1;
	}

	LLVMDisposeMessage(err);
	return 0;
}

LLVMModuleRef generate_llvm(const char *module_name, const char *main_func_name, char *file_contents) {
	LLVMModuleRef main_module = LLVMModuleCreateWithName(module_name);

	LLVMTypeRef main_func_type = LLVMFunctionType(
		LLVMInt64Type(),
		NULL,
		0,
		false
	);

	LLVMValueRef main_func = LLVMAddFunction(
		main_module,
		main_func_name,
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

	str_to_llvm_ir(
		file_contents,
		main_func,
		main_module,
		builder
	);

	LLVMDisposeBuilder(builder);

	return main_module;
}

char *create_llvm_filename(const char *filename) {
	const size_t len = strlen(filename);
	char *const llvm_filename = malloc(len + 5);

	const char *const slash_pos = strrchr(filename, '/');
	if (!slash_pos) {
		llvm_filename[0] = '.';
		memcpy(llvm_filename + 1, filename, len);
	}
	else {
		const long offset = slash_pos - filename;

		memcpy(llvm_filename, filename, len);
		llvm_filename[offset + 1] = '.';
		memcpy(llvm_filename + offset + 2, slash_pos + 1, len - (size_t)offset);
	}
	memcpy(llvm_filename + len + 1, ".ll", 4);

	return llvm_filename;
}
