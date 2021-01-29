; ModuleID = './test/sh/function/external_with_param.sk'
source_filename = "./test/sh/function/external_with_param.sk"

define i64 @.external_with_param() {
entry:
  ret i64 0
}

declare void @x(i64)
