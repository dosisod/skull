; ModuleID = './test/sh/function/call_str.sk'
source_filename = "./test/sh/function/call_str.sk"

@0 = private unnamed_addr constant [13 x i8] c"hello world!\00", align 1

define i64 @main() {
entry:
  call void @puts(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @0, i32 0, i32 0))
}

declare void @puts(i8*)
