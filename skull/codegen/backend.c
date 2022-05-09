#include <stdbool.h>

#include "skull/codegen/shared.h"

#include "skull/codegen/backend.h"

bool run_backend(
	SemanticState *semantic_state,
	const Backend *backend,
	const AstNode *node,
	const char *filename
) {
	void *state = NULL;
	if (backend->setup) state = backend->setup(semantic_state);

	if (backend->gen_module) backend->gen_module(semantic_state, node, state);

	const bool success = backend->write(filename, state);

	if (backend->cleanup) backend->cleanup(state);

	return success;
}
