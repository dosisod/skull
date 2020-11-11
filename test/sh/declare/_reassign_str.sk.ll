; ModuleID = './test/sh/declare/reassign_str.sk'
source_filename = "./test/sh/declare/reassign_str.sk"

@0 = private unnamed_addr constant [4 x i8] c"abc\00", align 1
@1 = private unnamed_addr constant [5 x i8] c"defg\00", align 1

define i64 @main() {
entry:
  %x = alloca i8*
  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i8** %x
  store i8* getelementptr inbounds ([5 x i8], [5 x i8]* @1, i32 0, i32 0), i8** %x
}
