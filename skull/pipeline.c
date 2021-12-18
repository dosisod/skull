#include <stdbool.h>
#include <stdlib.h>

#include "skull/build_data.h"
#include "skull/codegen/c/backend.h"
#include "skull/codegen/llvm/backend.h"
#include "skull/codegen/llvm/core.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/common/str.h"
#include "skull/parse/classify.h"
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
	char32_t *_file_contents = mbstoc32s(file_contents);
	if (!_file_contents) {
		free(file_contents);
		return true;
	}

	Token *token = tokenize(_file_contents);
	if (!token) {
		free(_file_contents);
		free(file_contents);
		return true;
	}

	classify_tokens(token);

	AstNode *node = parse_ast_tree(token);
	if (!node) {
		free(_file_contents);
		free(file_contents);
		return true;
	}

	setup_semantic_state();

	bool err = !validate_ast_tree(node);

	// For now, the LLVM backend will always be selected as the default.
	BUILD_DATA.llvm_backend = true;

	if (err) {}
	else if (BUILD_DATA.c_backend) {
		err = run_backend(&c_backend, node, filename);
	}
	else if (BUILD_DATA.llvm_backend) {
		err = run_backend(&llvm_backend, node, filename);
	}

	free_ast_tree(node);
	free(_file_contents);
	free(file_contents);
	free_semantic_state();

	return err;
}
