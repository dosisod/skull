; ModuleID = './test/sh/function/multi_param_func_different_types.sk'
source_filename = "./test/sh/function/multi_param_func_different_types.sk"

define i64 @main() {
entry:
  ret i64 0
}

define private void @f(i64 %0, double %1) {
entry:
  ret void
}
