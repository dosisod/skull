; ModuleID = './test/sh/declare_from_var.sk'
source_filename = "./test/sh/declare_from_var.sk"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 0, i64* %x
  %z = alloca i64
  store i64 0, i64* %z
}
