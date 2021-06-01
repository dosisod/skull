#pragma once

#include "skull/codegen/shared.h"
#include "skull/compiler/variable.h"
#include "skull/parse/token.h"

void make_sub_scope(Scope **, Scope **);
void restore_sub_scope(Scope **, Scope **);

Variable *scope_find_var(const Token *const);
