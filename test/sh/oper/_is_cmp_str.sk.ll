; ModuleID = './test/sh/oper/is_cmp_str.sk'
source_filename = "./test/sh/oper/is_cmp_str.sk"

@0 = private unnamed_addr constant [2 x i8] c"x\00", align 1
@1 = private unnamed_addr constant [2 x i8] c"x\00", align 1
@x = private global i1 false

define i64 @.is_cmp_str() {
entry:
  %0 = call i1 @_strcmp(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @0, i32 0, i32 0), i8* getelementptr inbounds ([2 x i8], [2 x i8]* @1, i32 0, i32 0))
  store i1 %0, i1* @x
  ret i64 0
}

declare i1 @_strcmp(i8*, i8*)
