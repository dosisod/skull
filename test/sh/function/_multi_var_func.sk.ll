; ModuleID = './test/sh/function/multi_var_func.sk'
source_filename = "./test/sh/function/multi_var_func.sk"

define i64 @.multi_var_func() {
entry:
  %0 = call i64 @f(i64 1)
  ret i64 0
}

define private i64 @f(i64 %0) {
entry:
  ret i64 %0
}
