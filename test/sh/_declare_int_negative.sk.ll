; ModuleID = './test/sh/declare_int_negative.sk'
source_filename = "./test/sh/declare_int_negative.sk"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 -1234, i64* %x
}
