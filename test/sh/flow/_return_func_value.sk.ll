; ModuleID = './test/sh/flow/return_func_value.sk'
source_filename = "./test/sh/flow/return_func_value.sk"

define i64 @main() {
entry:
  %0 = call i64 @f()
  ret i64 %0
}

declare i64 @f()
