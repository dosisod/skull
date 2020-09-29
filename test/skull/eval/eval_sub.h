#pragma once

#define eval_sub(x, y) eval_oper((x)->type->subtract, x, y)

void eval_sub_test_self(_Bool *pass);
