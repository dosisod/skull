; ModuleID = './test/sh/oper/is_cmp_str.sk'
source_filename = "./test/sh/oper/is_cmp_str.sk"

@0 = private unnamed_addr constant [2 x i8] c"x\00", align 1
@1 = private unnamed_addr constant [2 x i8] c"x\00", align 1
@x = private global i1 false

define i64 @.is_cmp_str() {
entry:
  %0 = call i1 @_strcmp(ptr @0, ptr @1)
  store i1 %0, ptr @x, align 1
  ret i64 0
}

declare i1 @_strcmp(ptr, ptr)
