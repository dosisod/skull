#pragma once

#define eval_div(x, y) eval_oper((x)->type->divide, x, y)

void eval_div_test_self(_Bool *);
