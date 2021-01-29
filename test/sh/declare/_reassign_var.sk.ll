; ModuleID = './test/sh/declare/reassign_var.sk'
source_filename = "./test/sh/declare/reassign_var.sk"

@x = private global i64 0

define i64 @.reassign_var() {
entry:
  store i64 0, i64* @x
  store i64 1, i64* @x
  ret i64 0
}
