; ModuleID = './test/sh/function/expr_param.sk'
source_filename = "./test/sh/function/expr_param.sk"

define i64 @main() {
entry:
  call void @f(i64 3)
  ret i64 0
}

define private void @f(i64 %0) {
entry:
  ret void
}
