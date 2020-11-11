; ModuleID = './test/sh/declare/many.sk'
source_filename = "./test/sh/declare/many.sk"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 0, i64* %x
  %y = alloca i64
  store i64 0, i64* %y
}
