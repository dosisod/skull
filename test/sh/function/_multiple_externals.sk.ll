; ModuleID = './test/sh/function/multiple_externals.sk'
source_filename = "./test/sh/function/multiple_externals.sk"

define i64 @.multiple_externals() {
entry:
  call void @a()
  ret i64 0
}

declare void @a()

declare void @b()
