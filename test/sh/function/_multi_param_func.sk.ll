; ModuleID = './test/sh/function/multi_param_func.sk'
source_filename = "./test/sh/function/multi_param_func.sk"

define i64 @.multi_param_func() {
entry:
  %0 = call i64 @f(i64 1, i64 2)
  ret i64 0
}

define private i64 @f(i64 %0, i64 %1) {
entry:
  %2 = add nsw i64 %0, %1
  ret i64 %2
}
