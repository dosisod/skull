; ModuleID = './test/sh/declare_rune.sk'
source_filename = "./test/sh/declare_rune.sk"

define i64 @main() {
entry:
  %x = alloca i32
  store i32 65, i32* %x
}
