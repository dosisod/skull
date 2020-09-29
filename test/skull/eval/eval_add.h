#pragma once

#define eval_add(x, y) eval_oper((x)->type->add, x, y)

void eval_add_test_self(_Bool *);
