; ModuleID = './test/sh/oper/unary_expr.sk'
source_filename = "./test/sh/oper/unary_expr.sk"

@w = private global double -1.000000e+00
@x = private global i1 true
@y = private global i64 -1
@z = private global i64 0

define i64 @.unary_expr() {
entry:
  %0 = load i64, i64* @y, align 4
  %1 = sub nsw i64 0, %0
  store i64 %1, i64* @z, align 4
  ret i64 0
}
