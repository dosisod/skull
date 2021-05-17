; ModuleID = './test/sh/flow/unreachable_in_func.sk'
source_filename = "./test/sh/flow/unreachable_in_func.sk"

define i64 @.unreachable_in_func() {
entry:
  ret i64 0
}

define private void @f() {
entry:
  unreachable
}
