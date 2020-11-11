; ModuleID = './test/sh/declare/rune_escaped.sk'
source_filename = "./test/sh/declare/rune_escaped.sk"

define i64 @main() {
entry:
  %x = alloca i32
  store i32 10, i32* %x
}
