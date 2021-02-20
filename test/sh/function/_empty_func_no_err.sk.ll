; ModuleID = './test/sh/function/empty_func_no_err.sk'
source_filename = "./test/sh/function/empty_func_no_err.sk"

define i64 @.empty_func_no_err() {
entry:
  ret i64 0
}

define private void @x() {
entry:
  ret void
}
