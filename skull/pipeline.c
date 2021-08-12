#include <stdbool.h>
#include <stdlib.h>

#include "skull/build_data.h"
#include "skull/codegen/ast.h"
#include "skull/codegen/llvm/debug.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/codegen/llvm/write.h"
#include "skull/codegen/write.h"
#include "skull/common/str.h"
#include "skull/pipeline.h"
#include "skull/semantic/entry.h"
#include "skull/semantic/shared.h"


#include "skull/pipeline.h"

/*
Run "parse" > "validate" > "codegen" pipeline on `file_contents`.

Function takes ownership of `file_contents`.

Return `true` if errors occurred.
*/
int run_pipeline(const char *filename, char *file_contents) {
	BUILD_DATA.filename = filename;

	char32_t *_file_contents = mbstoc32s(file_contents);
	if (!_file_contents) {
		free(file_contents);
		return true;
	}

	AstNode *node = parse_ast_tree(_file_contents);
	if (!node) {
		free(_file_contents);
		free(file_contents);
		return true;
	}

	bool err = true;

	setup_semantic_state();

	if (validate_ast_tree(node)) {
		setup_llvm_state();

		if (BUILD_DATA.debug) {
			setup_debug_info(filename, SKULL_STATE_LLVM.module);
		}

		err = gen_tree(node);
	}

	free_ast_tree(node);
	free(_file_contents);
	free(file_contents);

	if (!err) err = write_file(filename);

	free_llvm_state();
	free_semantic_state();

	return err;
}
