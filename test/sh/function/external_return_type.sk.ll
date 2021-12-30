; ModuleID = './test/sh/function/external_return_type.sk'
source_filename = "./test/sh/function/external_return_type.sk"

define i64 @.external_return_type() {
entry:
  ret i64 0
}

declare i64 @x()

declare i64 @y(i64)
