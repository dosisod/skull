; ModuleID = './test/sh/declare/basic_types.sk'
source_filename = "./test/sh/declare/basic_types.sk"

@bool = private global i1 true
@int = private global i64 1234
@float = private global double 3.140000e+00
@0 = private unnamed_addr constant [12 x i8] c"hello world\00", align 1
@str = private global i8* getelementptr inbounds ([12 x i8], [12 x i8]* @0, i32 0, i32 0)
@rune = private global i32 120

define i64 @.basic_types() {
entry:
  ret i64 0
}
