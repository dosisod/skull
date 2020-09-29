#pragma once

#define eval_mult(x, y) eval_oper((x)->type->multiply, x, y)

void eval_mult_test_self(_Bool *pass);
