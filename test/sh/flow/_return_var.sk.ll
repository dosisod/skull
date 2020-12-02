; ModuleID = './test/sh/flow/return_var.sk'
source_filename = "./test/sh/flow/return_var.sk"

@x = private global i64 0

define i64 @main() {
entry:
  store i64 1, i64* @x
  %0 = load i64, i64* @x
  ret i64 %0
}
