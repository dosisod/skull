#pragma once

typedef struct AstNode AstNode;

_Bool validate_stmt_return(const AstNode *);
_Bool validate_stmt_unreachable(const AstNode *);
_Bool validate_control_if(const AstNode *);
_Bool validate_control_elif(const AstNode *);
_Bool validate_control_else(const AstNode *);
_Bool validate_control_while(const AstNode *);
_Bool validate_control_break(const AstNode *);
_Bool validate_control_continue(const AstNode *);
_Bool assert_sane_child(const AstNode *);
