; ModuleID = './test/sh/oper/lhs_with_var.sk'
source_filename = "./test/sh/oper/lhs_with_var.sk"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 1, i64* %x
  %z = alloca i64
  %0 = load i64, i64* %x
  %1 = add i64 %0, 2
  store i64 %1, i64* %z
}
