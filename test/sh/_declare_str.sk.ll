; ModuleID = './test/sh/declare_str.sk'
source_filename = "./test/sh/declare_str.sk"

@0 = private unnamed_addr constant [7 x i8] c"abcdef\00", align 1

define i64 @main() {
entry:
  %x = alloca i8*
  store i8* getelementptr inbounds ([7 x i8], [7 x i8]* @0, i32 0, i32 0), i8** %x
}
