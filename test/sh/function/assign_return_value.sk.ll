; ModuleID = './test/sh/function/assign_return_value.sk'
source_filename = "./test/sh/function/assign_return_value.sk"

@x = private global i64 0

define i64 @.assign_return_value() {
entry:
  %0 = call i64 @func()
  store i64 %0, ptr @x, align 4
  %1 = load i64, ptr @x, align 4
  ret i64 %1
}

declare i64 @func()
