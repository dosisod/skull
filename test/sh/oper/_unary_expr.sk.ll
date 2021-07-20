; ModuleID = './test/sh/oper/unary_expr.sk'
source_filename = "./test/sh/oper/unary_expr.sk"

@w = private global double 0.000000e+00
@x = private global i1 false
@y = private global i64 0
@z = private global i64 0

define i64 @.unary_expr() {
entry:
  store double -1.000000e+00, double* @w
  store i1 false, i1* @x
  store i64 -1, i64* @y
  %0 = load i64, i64* @y
  %1 = sub nsw i64 0, %0
  store i64 %1, i64* @z
  ret i64 0
}
