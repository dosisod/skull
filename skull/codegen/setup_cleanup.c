#include <stdio.h>
#include <stdlib.h>

#include <llvm-c/Core.h>

#include "skull/codegen/ast.h"
#include "skull/codegen/func.h"
#include "skull/codegen/shared.h"

#include "skull/codegen/setup_cleanup.h"

/*
Write LLVM code to `filename`, return whether error occured.

Function takes control of `filename`.
*/
bool write_file(char *filename) {
	char *err = NULL;
	LLVMBool status = LLVMPrintModuleToFile(
		SKULL_STATE.module,
		filename,
		&err
	);

	free_state(&SKULL_STATE);
	free(filename);

	if (err || status) {
		fprintf(stderr, "skull: error occurred: %s\n", err);
		LLVMDisposeMessage(err);
		return 1;
	}

	LLVMDisposeMessage(err);
	return 0;
}

/*
Create a module named `filename` from `file_contents`.

Return `true` if errors occurred.
*/
bool generate_llvm(const char *filename, char *file_contents) {
	setup_state(&SKULL_STATE, filename);

	return codegen_str(file_contents);
}
