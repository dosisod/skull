; ModuleID = './test/sh/declare/rune_escaped.sk'
source_filename = "./test/sh/declare/rune_escaped.sk"

@x = private global i32 0

define i64 @.rune_escaped() {
entry:
  store i32 10, i32* @x
  ret i64 0
}
