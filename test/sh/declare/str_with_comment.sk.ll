; ModuleID = './test/sh/declare/str_with_comment.sk'
source_filename = "./test/sh/declare/str_with_comment.sk"

@0 = private unnamed_addr constant [3 x i8] c"# \00", align 1
@x = private global i8* getelementptr inbounds ([3 x i8], [3 x i8]* @0, i32 0, i32 0)

define i64 @.str_with_comment() {
entry:
  ret i64 0
}
