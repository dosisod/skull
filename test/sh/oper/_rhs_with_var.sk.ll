; ModuleID = './test/sh/oper/rhs_with_var.sk'
source_filename = "./test/sh/oper/rhs_with_var.sk"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 1, i64* %x
  %z = alloca i64
  %0 = load i64, i64* %x
  %1 = add i64 2, %0
  store i64 %1, i64* %z
  ret i64 0
}
