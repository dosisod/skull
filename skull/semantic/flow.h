#pragma once

#include "skull/semantic/shared.h"

typedef struct AstNode AstNode;

_Bool validate_stmt_return(SemanticState *, const AstNode *);
_Bool validate_stmt_unreachable(const AstNode *);
_Bool validate_control_if(SemanticState *, const AstNode *);
_Bool validate_control_elif(SemanticState *, const AstNode *);
_Bool validate_control_else(SemanticState *, const AstNode *);
_Bool validate_control_while(SemanticState *, const AstNode *);
_Bool validate_control_break(SemanticState *, const AstNode *);
_Bool validate_control_continue(SemanticState *, const AstNode *);
_Bool assert_sane_child(const AstNode *);
