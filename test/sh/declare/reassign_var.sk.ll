; ModuleID = './test/sh/declare/reassign_var.sk'
source_filename = "./test/sh/declare/reassign_var.sk"

@x = private global i64 0

define i64 @.reassign_var() {
entry:
  store i64 1, ptr @x, align 4
  ret i64 0
}
