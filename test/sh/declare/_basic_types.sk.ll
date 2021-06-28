; ModuleID = './test/sh/declare/basic_types.sk'
source_filename = "./test/sh/declare/basic_types.sk"

@bool = private global i1 false
@int = private global i64 0
@float = private global double 0.000000e+00
@0 = private unnamed_addr constant [12 x i8] c"hello world\00", align 1
@str = private global i8* null
@rune = private global i32 0

define i64 @.basic_types() {
entry:
  store i1 true, i1* @bool
  store i64 1234, i64* @int
  store double 3.140000e+00, double* @float
  store i8* getelementptr inbounds ([12 x i8], [12 x i8]* @0, i32 0, i32 0), i8** @str
  store i32 120, i32* @rune
  ret i64 0
}
