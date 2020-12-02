; ModuleID = './test/sh/declare/rune_null.sk'
source_filename = "./test/sh/declare/rune_null.sk"

@x = private global i32 0

define i64 @main() {
entry:
  store i32 0, i32* @x
  ret i64 0
}
