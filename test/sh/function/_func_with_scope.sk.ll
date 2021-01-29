; ModuleID = './test/sh/function/func_with_scope.sk'
source_filename = "./test/sh/function/func_with_scope.sk"

@x = private global i64 0

define i64 @.func_with_scope() {
entry:
  store i64 0, i64* @x
  ret i64 0
}

define private void @f() {
entry:
  %x = alloca i64
  store i64 1, i64* %x
  ret void
}
