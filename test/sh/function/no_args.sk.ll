; ModuleID = './test/sh/function/no_args.sk'
source_filename = "./test/sh/function/no_args.sk"

define i64 @.no_args() {
entry:
  call void @x()
  ret i64 0
}

declare void @x()
