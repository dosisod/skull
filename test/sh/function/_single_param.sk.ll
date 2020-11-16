; ModuleID = './test/sh/function/single_param.sk'
source_filename = "./test/sh/function/single_param.sk"

define i64 @main() {
entry:
  call void @x(i64 1)
  ret i64 0
}

declare void @x(i64)
