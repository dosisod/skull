; ModuleID = './test/sh/function/ref_type.sk'
source_filename = "./test/sh/function/ref_type.sk"

define i64 @.ref_type() {
entry:
  ret i64 0
}

define private void @f(ptr %0) {
entry:
  ret void
}
