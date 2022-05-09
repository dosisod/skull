#pragma once

#include "skull/parse/ast_node.h"

typedef struct Backend {
	void *(*setup)(SemanticState *);
	_Bool (*write)(const char *, void *);
	void (*gen_module)(SemanticState *, const AstNode *, void *);
	void (*cleanup)(void *);
	const char *extension;
} Backend;

_Bool run_backend(
	SemanticState *,
	const Backend *,
	const AstNode *,
	const char *
);
