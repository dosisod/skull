; ModuleID = './test/sh/function/multiple_externals.sk'
source_filename = "./test/sh/function/multiple_externals.sk"

define i64 @main() {
entry:
  call void @a()
}

declare void @a()

declare void @b()
