; ModuleID = './test/sh/declare/reassign_str.sk'
source_filename = "./test/sh/declare/reassign_str.sk"

@0 = private unnamed_addr constant [4 x i8] c"abc\00", align 1
@x = private global ptr @0
@1 = private unnamed_addr constant [5 x i8] c"defg\00", align 1

define i64 @.reassign_str() {
entry:
  store ptr @1, ptr @x, align 8
  ret i64 0
}
