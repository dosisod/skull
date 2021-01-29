; ModuleID = './test/sh/declare/float_zero.sk'
source_filename = "./test/sh/declare/float_zero.sk"

@x = private global double 0.000000e+00

define i64 @.float_zero() {
entry:
  store double 0.000000e+00, double* @x
  ret i64 0
}
