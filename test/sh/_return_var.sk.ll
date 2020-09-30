; ModuleID = './test/sh/return_var.sk'
source_filename = "./test/sh/return_var.sk"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 1, i64* %x
  %0 = load i64, i64* %x
  ret i64 %0
}
