; ModuleID = './test/sh/misc/escape_sequences.sk'
source_filename = "./test/sh/misc/escape_sequences.sk"

@0 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@x = private global i8* null
@y = private global i32 0

define i64 @.escape_sequences() {
entry:
  store i8* getelementptr inbounds ([2 x i8], [2 x i8]* @0, i32 0, i32 0), i8** @x
  store i32 10, i32* @y
  ret i64 0
}
