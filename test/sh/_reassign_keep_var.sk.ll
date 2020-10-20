; ModuleID = './test/sh/reassign_keep_var.sk'
source_filename = "./test/sh/reassign_keep_var.sk"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 0, i64* %x
  %0 = load i64, i64* %x
  store i64 %0, i64* %x
}
