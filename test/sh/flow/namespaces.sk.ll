; ModuleID = './test/sh/flow/namespaces.sk'
source_filename = "./test/sh/flow/namespaces.sk"

@z = private global i64 1
@num = private global i64 123

define i64 @.namespaces() {
entry:
  store i64 2, ptr @z, align 4
  call void @f()
  ret i64 0
}

define private void @f() {
entry:
  ret void
}
