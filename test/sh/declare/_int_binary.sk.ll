; ModuleID = './test/sh/declare/int_binary.sk'
source_filename = "./test/sh/declare/int_binary.sk"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 21, i64* %x
}
