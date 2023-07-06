; ModuleID = './test/sh/declare/ptr_types.sk'
source_filename = "./test/sh/declare/ptr_types.sk"

@bool = private global i1 true
@int = private global i64 1234
@float = private global double 3.140000e+00
@0 = private unnamed_addr constant [12 x i8] c"hello world\00", align 1
@str = private global ptr @0
@rune = private global i32 120
@bool_ptr = private global ptr @bool
@int_ptr = private global ptr @int
@float_ptr = private global ptr @float
@str_ptr = private global ptr @str
@rune_ptr = private global ptr @rune
@double_ptr = private global ptr @int_ptr

define i64 @.ptr_types() {
entry:
  ret i64 0
}
