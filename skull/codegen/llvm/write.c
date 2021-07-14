#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <llvm-c/Core.h>
#include <llvm-c/DebugInfo.h>

#include "skull/build_data.h"
#include "skull/codegen/llvm/debug.h"
#include "skull/codegen/llvm/shared.h"

#include "skull/codegen/llvm/write.h"

/*
Write LLVM code to `filename`, return whether error occured.
*/
bool write_file_llvm(char *filename) {
	if (BUILD_DATA.debug) LLVMDIBuilderFinalize(DEBUG_INFO.builder);

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
