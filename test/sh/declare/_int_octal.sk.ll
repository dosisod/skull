; ModuleID = './test/sh/declare/int_octal.sk'
source_filename = "./test/sh/declare/int_octal.sk"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 511, i64* %x
}
