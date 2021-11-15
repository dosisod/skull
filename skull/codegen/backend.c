#include <stdbool.h>

#include "skull/codegen/backend.h"

_Bool run_backend(
	const Backend *backend,
	const AstNode *node,
	const char *filename
) {
	if (backend->setup) backend->setup();

	if (backend->gen_module) backend->gen_module(node);

	return backend->write(filename);
}
