; ModuleID = './test/sh/misc/triple_param_no_overflow.sk'
source_filename = "./test/sh/misc/triple_param_no_overflow.sk"

define i64 @.triple_param_no_overflow() {
entry:
  ret i64 0
}

define private void @f(i64 %0, i64 %1, i64 %2) {
entry:
  ret void
}
