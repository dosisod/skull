#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/color.h"
#include "skull/common/errors.h"
#include "skull/common/io.h"
#include "skull/common/local.h"
#include "skull/common/malloc.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/llvm/ast.h"
#include "skull/llvm/func.h"
#include "skull/llvm/shared.h"
#include "skull/setup_main.h"

#define DIE(x) puts(x); return 1

/*
Actual `main` function, can be called by external programs.
*/
int setup_main(int argc, char *argv[]) {
	SETUP_LOCALE();

	if (argc < 2) {
		DIE("unexpected number of parameters");
	}

	for (int i = 0; i < argc - 1 ; i++) {
		const int err = build_file(argv[i + 1]);
		if (err) {
			return err;
		}
	}

	return 0;
}

/*
Build a .ll file from a .sk file `filename`.
*/
int build_file(char *filename) {
	if (!strrstr(filename, ".sk")) {
		DIE("missing required \".sk\" extension, exiting");
	}

	if (strrstr(filename, ".sk") == filename || strrstr(filename, "/.sk")) {
		DIE("\".sk\" is not a valid name, exiting");
	}

	errno = 0;
	FILE *const f = fopen(filename, "re");
	if (!f) {
		if (errno == EACCES) {
			printf("cannot open \"%s\", permission denied\n", filename);
		}
		else if (errno == ENOENT) {
			printf("\"%s\" was not found, exiting\n", filename);
		}
		return 1;
	}

	char *const file_contents = read_file(f);
	if (!file_contents) {
		PANIC(ERR_FILE_EMPTY, {0});
	}
	fclose(f);

	char *main_func_name = create_llvm_main_func(filename);

	generate_llvm(
		filename,
		main_func_name,
		file_contents
	);
	free(file_contents);
	free(main_func_name);

	char *llvm_filename = create_llvm_filename(filename);

	char *err = NULL;
	LLVMBool status = LLVMPrintModuleToFile(
		SKULL_STATE.module,
		llvm_filename,
		&err
	);

	free_state(SKULL_STATE);
	free(llvm_filename);

	if (err || status) {
		printf("error occurred: %s\n", err);
		LLVMDisposeMessage(err);
		return 1;
	}

	LLVMDisposeMessage(err);
	return 0;
}

/*
Create a module named `module_name` and a main function called
`main_func_name` from `file_contents`.
*/
void generate_llvm(
	const char *module_name,
	const char *main_func_name,
	char *file_contents
) {
	LLVMModuleRef main_module = LLVMModuleCreateWithName(module_name);

	LLVMContextRef ctx = LLVMContextCreate();

	LLVMTypeRef main_func_type = LLVMFunctionType(
		LLVMInt64TypeInContext(ctx),
		NULL,
		0,
		false
	);

	LLVMValueRef main_func = LLVMAddFunction(
		main_module,
		main_func_name,
		main_func_type
	);

	LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(
		ctx,
		main_func,
		"entry"
	);

	LLVMBuilderRef builder = LLVMCreateBuilderInContext(ctx);

	LLVMPositionBuilderAtEnd(
		builder,
		entry
	);

	SKULL_STATE = (SkullState){
		.builder = builder,
		.ctx = ctx,
		.module = main_module,
		.current_func = main_func,
		.main_func = main_func,
		.scope = make_scope(),
		.function_decls = ht_create(),
		.type_aliases = ht_create(),
		.template_types = ht_create()
	};

	str_to_llvm_ir(file_contents);
}

/*
Convert/mangle `filename` into suitable name for "main" method for module.
*/
char *create_llvm_main_func(const char *filename) {
	char *slash_pos = strrchr(filename, '/');

	if (slash_pos) {
		filename = slash_pos + 1;
	}

	const size_t len = strlen(filename) - 1;

	char *ret = Malloc(len);
	ret[0] = '.';
	strncpy(ret + 1, filename, len - 1);
	ret[len - 1] = '\0';

	return ret;
}

/*
Modify filename to add `.ll` extention to it.
*/
char *create_llvm_filename(const char *filename) {
	const size_t len = strlen(filename);
	char *const llvm_filename = Malloc(len + 5);

	const char *const slash_pos = strrchr(filename, '/');
	if (!slash_pos) {
		llvm_filename[0] = '.';
		strncpy(llvm_filename + 1, filename, len);
	}
	else {
		const long offset = slash_pos - filename;

		strncpy(llvm_filename, filename, len);
		llvm_filename[offset + 1] = '.';
		strncpy(
			llvm_filename + offset + 2,
			slash_pos + 1, len - (size_t)offset
		);
	}
	strncpy(llvm_filename + len + 1, ".ll", 4);

	return llvm_filename;
}
