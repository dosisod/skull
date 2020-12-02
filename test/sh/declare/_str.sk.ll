; ModuleID = './test/sh/declare/str.sk'
source_filename = "./test/sh/declare/str.sk"

@x = private global i8* null
@0 = private unnamed_addr constant [7 x i8] c"abcdef\00", align 1

define i64 @main() {
entry:
  store i8* getelementptr inbounds ([7 x i8], [7 x i8]* @0, i32 0, i32 0), i8** @x
  ret i64 0
}
