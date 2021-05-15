; ModuleID = './test/sh/function/void_explicit_ret.sk'
source_filename = "./test/sh/function/void_explicit_ret.sk"

define i64 @.void_explicit_ret() {
entry:
  ret i64 0
}

define private void @f() {
entry:
  ret void
}
