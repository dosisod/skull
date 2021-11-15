#pragma once

#include "skull/parse/ast_node.h"

typedef struct Backend {
	void (*setup)(void);
	_Bool (*write)(const char *);
	void (*gen_module)(const AstNode *);
	void (*cleanup)(void);
} Backend;

_Bool run_backend(const Backend *, const AstNode *, const char *);
