; ModuleID = './test/sh/function/exported.sk'
source_filename = "./test/sh/function/exported.sk"

define i64 @.exported() {
entry:
  call void @exported.f()
  ret i64 0
}

define void @exported.f() {
entry:
  ret void
}
