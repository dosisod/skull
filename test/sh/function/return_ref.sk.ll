; ModuleID = './test/sh/function/return_ref.sk'
source_filename = "./test/sh/function/return_ref.sk"

@i = private global i64 1

define i64 @.return_ref() {
entry:
  ret i64 0
}

define private i64* @f() {
entry:
  ret i64* @i
}
