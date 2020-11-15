; ModuleID = './test/sh/declare/rune_null.sk'
source_filename = "./test/sh/declare/rune_null.sk"

define i64 @main() {
entry:
  %x = alloca i32
  store i32 0, i32* %x
}
