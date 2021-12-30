; ModuleID = './test/sh/oper/all_exprs.sk'
source_filename = "./test/sh/oper/all_exprs.sk"

@bool = private global i1 true
@int = private global i64 1234
@float = private global double 3.140000e+00
@0 = private unnamed_addr constant [12 x i8] c"hello world\00", align 1
@str = private global i8* getelementptr inbounds ([12 x i8], [12 x i8]* @0, i32 0, i32 0)
@rune = private global i32 120
@not_ = private global i1 true
@and_ = private global i1 true
@or_ = private global i1 true
@xor_ = private global i1 true
@greater_than_eq_int = private global i1 true
@greater_than_eq_float = private global i1 true
@gtr_than_int = private global i1 true
@gtr_than_float = private global i1 true
@is_int = private global i1 true
@is_float = private global i1 true
@is_rune = private global i1 true
@is_bool = private global i1 true
@isnt_int = private global i1 false
@isnt_float = private global i1 false
@isnt_rune = private global i1 false
@isnt_bool = private global i1 false
@less_than_eq_int = private global i1 true
@less_than_eq_float = private global i1 true
@less_than_int = private global i1 true
@less_than_float = private global i1 true
@mult_float = private global double 6.000000e+00
@mult_int = private global i64 6
@div_float = private global double 2.000000e+00
@div_int = private global i64 2
@add_int = private global i64 3
@add_float = private global double 5.000000e+00
@subtract_int = private global i64 -1
@subtract_float = private global double -1.000000e+00
@lshift = private global i64 4
@rshift = private global i64 3
@mod_int = private global i64 1
@mod_negative_int = private global i64 -1
@mod_float = private global double 2.000000e+00

define i64 @.all_exprs() {
entry:
  ret i64 0
}
