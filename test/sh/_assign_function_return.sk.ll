; ModuleID = './test/sh/assign_function_return.sk'
source_filename = "./test/sh/assign_function_return.sk"

define i64 @main() {
entry:
  %x = alloca i64
  %0 = call i64 @func()
  store i64 %0, i64* %x
  %1 = load i64, i64* %x
  ret i64 %1
}

declare i64 @func()
