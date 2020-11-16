; ModuleID = './test/sh/function/multi_var_func.sk'
source_filename = "./test/sh/function/multi_var_func.sk"

define i64 @main() {
entry:
  %0 = call i64 @f(i64 1)
  ret i64 0
}

define i64 @f(i64) {
entry:
  ret i64 %0
}
