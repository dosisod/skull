#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <llvm-c/Core.h>

#include "skull/codegen/ast.h"
#include "skull/codegen/func.h"
#include "skull/codegen/shared.h"

#include "skull/codegen/llvm/write.h"

/*
Write LLVM code to `filename`, return whether error occured.
*/
bool write_file_llvm(char *filename) {
	char *msg = NULL;
	LLVMBool did_fail = LLVMPrintModuleToFile(
		SKULL_STATE.module,
		filename,
		&msg
	);

	if (did_fail) fprintf(stderr, "skull: error occurred: %s\n", msg);

	LLVMDisposeMessage(msg);
	return did_fail;
}
