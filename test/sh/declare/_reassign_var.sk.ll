; ModuleID = './test/sh/declare/reassign_var.sk'
source_filename = "./test/sh/declare/reassign_var.sk"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 0, i64* %x
  store i64 1, i64* %x
  ret i64 0
}