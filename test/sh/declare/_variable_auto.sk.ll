; ModuleID = './test/sh/declare/variable_auto.sk'
source_filename = "./test/sh/declare/variable_auto.sk"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 0, i64* %x
  %z = alloca i64
  %0 = load i64, i64* %x
  store i64 %0, i64* %z
  %1 = load i64, i64* %z
  ret i64 %1
}
