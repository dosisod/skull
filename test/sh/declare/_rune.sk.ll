; ModuleID = './test/sh/declare/rune.sk'
source_filename = "./test/sh/declare/rune.sk"

define i64 @main() {
entry:
  %x = alloca i32
  store i32 65, i32* %x
  ret i64 0
}
