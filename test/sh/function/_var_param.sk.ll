; ModuleID = './test/sh/function/var_param.sk'
source_filename = "./test/sh/function/var_param.sk"

@num = private global i64 0

define i64 @.var_param() {
entry:
  store i64 1234, i64* @num
  %0 = load i64, i64* @num
  call void @f(i64 %0)
  ret i64 0
}

declare void @f(i64)
