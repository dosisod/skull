; ModuleID = './test/sh/function/simple.sk'
source_filename = "./test/sh/function/simple.sk"

define i64 @.simple() {
entry:
  call void @x()
  ret i64 0
}

declare void @x()
