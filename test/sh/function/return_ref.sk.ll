; ModuleID = './test/sh/function/return_ref.sk'
source_filename = "./test/sh/function/return_ref.sk"

@i = private global i64 1

define i64 @.return_ref() {
entry:
  ret i64 0
}

define private ptr @f() {
entry:
  ret ptr @i
}
