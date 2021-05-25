#include <stdbool.h>
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
	char *msg = NULL;
	LLVMBool did_fail = LLVMPrintModuleToFile(
		SKULL_STATE.module,
		filename,
		&msg
	);

	free_state(&SKULL_STATE);
	free(filename);

	if (did_fail) fprintf(stderr, "skull: error occurred: %s\n", msg);

	LLVMDisposeMessage(msg);
	return did_fail;
}
