#pragma once

#include "skull/parse/ast_node.h"

typedef struct Backend {
	void *(*setup)(void);
	_Bool (*write)(const char *, void *);
	void (*gen_module)(const AstNode *, void *);
	void (*cleanup)(void *);
	const char *extension;
} Backend;

_Bool run_backend(const Backend *, const AstNode *, const char *);
