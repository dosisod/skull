; ModuleID = './test/sh/declare/int_hex.sk'
source_filename = "./test/sh/declare/int_hex.sk"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 255, i64* %x
}
