; ModuleID = './test/sh/function/call_str.sk'
source_filename = "./test/sh/function/call_str.sk"

@0 = private unnamed_addr constant [13 x i8] c"hello world!\00", align 1

define i64 @.call_str() {
entry:
  call void @puts(ptr @0)
  ret i64 0
}

declare void @puts(ptr)
