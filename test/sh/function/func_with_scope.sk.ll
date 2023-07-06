; ModuleID = './test/sh/function/func_with_scope.sk'
source_filename = "./test/sh/function/func_with_scope.sk"

@x = private global i64 0

define i64 @.func_with_scope() {
entry:
  ret i64 0
}

define private void @f() {
entry:
  %x = alloca i64, align 8
  store i64 1, ptr %x, align 4
  ret void
}
