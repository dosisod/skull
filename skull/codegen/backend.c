#include <stdbool.h>

#include "skull/codegen/backend.h"

bool run_backend(
	const Backend *backend,
	const AstNode *node,
	const char *filename
) {
	if (backend->setup) backend->setup();

	if (backend->gen_module) backend->gen_module(node);

	const bool success = backend->write(filename);

	if (backend->cleanup) backend->cleanup();

	return success;
}
