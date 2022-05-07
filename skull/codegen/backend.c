#include <stdbool.h>

#include "skull/codegen/shared.h"

#include "skull/codegen/backend.h"

bool run_backend(
	const Backend *backend,
	const AstNode *node,
	const char *filename
) {
	void *state = NULL;
	if (backend->setup) state = backend->setup();

	if (backend->gen_module) backend->gen_module(node, state);

	const bool success = backend->write(filename, state);

	if (backend->cleanup) backend->cleanup(state);

	return success;
}
