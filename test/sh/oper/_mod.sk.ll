; ModuleID = './test/sh/oper/mod.sk'
source_filename = "./test/sh/oper/mod.sk"

@x = private global i64 0
@y = private global i64 0
@z = private global double 0.000000e+00

define i64 @.mod() {
entry:
  store i64 1, i64* @x
  store i64 -1, i64* @y
  store double 2.000000e+00, double* @z
  ret i64 0
}
