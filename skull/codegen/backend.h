#pragma once

#include "skull/parse/ast_node.h"

typedef struct Backend {
	void (*setup)(void);
	_Bool (*write)(const char *);
	void (*gen_module)(AstNode *);
	void (*cleanup)(void);
} Backend;

_Bool run_backend(const Backend *, AstNode *, const char *);
