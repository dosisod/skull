; ModuleID = './test/sh/simple_function_call.sk'
source_filename = "./test/sh/simple_function_call.sk"

define i64 @main() {
entry:
  call void @x()
}

declare void @x()
