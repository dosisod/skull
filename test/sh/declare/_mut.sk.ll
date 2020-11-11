; ModuleID = './test/sh/declare/mut.sk'
source_filename = "./test/sh/declare/mut.sk"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 0, i64* %x
}
