; ModuleID = './test/sh/function/func_expr_param.sk'
source_filename = "./test/sh/function/func_expr_param.sk"

define i64 @.func_expr_param() {
entry:
  %0 = call i64 @f(i64 1)
  %1 = call i64 @f(i64 %0)
  ret i64 0
}

define private i64 @f(i64 %0) {
entry:
  ret i64 %0
}
