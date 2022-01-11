; ModuleID = './test/sh/declare/ptr_types.sk'
source_filename = "./test/sh/declare/ptr_types.sk"

@bool = private global i1 true
@int = private global i64 1234
@float = private global double 3.140000e+00
@0 = private unnamed_addr constant [12 x i8] c"hello world\00", align 1
@str = private global i8* getelementptr inbounds ([12 x i8], [12 x i8]* @0, i32 0, i32 0)
@rune = private global i32 120
@bool_ptr = private global i1* @bool
@int_ptr = private global i64* @int
@float_ptr = private global double* @float
@str_ptr = private global i8** @str
@rune_ptr = private global i32* @rune
@double_ptr = private global i64** @int_ptr

define i64 @.ptr_types() {
entry:
  ret i64 0
}
