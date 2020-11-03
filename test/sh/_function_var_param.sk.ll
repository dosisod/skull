; ModuleID = './test/sh/function_var_param.sk'
source_filename = "./test/sh/function_var_param.sk"

define i64 @main() {
entry:
  %num = alloca i64
  store i64 1234, i64* %num
  %0 = load i64, i64* %num
  call void @f(i64 %0)
}

declare void @f(i64)
