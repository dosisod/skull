; ModuleID = './test/sh/declare/int.sk'
source_filename = "./test/sh/declare/int.sk"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 1234, i64* %x
}
