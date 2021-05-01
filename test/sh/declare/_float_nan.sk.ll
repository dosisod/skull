; ModuleID = './test/sh/declare/float_nan.sk'
source_filename = "./test/sh/declare/float_nan.sk"

@x = private global double 0.000000e+00

define i64 @.float_nan() {
entry:
  store double 0x7FF8000000000000, double* @x
  ret i64 0
}
