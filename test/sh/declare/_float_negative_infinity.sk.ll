; ModuleID = './test/sh/declare/float_negative_infinity.sk'
source_filename = "./test/sh/declare/float_negative_infinity.sk"

@x = private global double 0.000000e+00

define i64 @.float_negative_infinity() {
entry:
  store double 0xFFF0000000000000, double* @x
  ret i64 0
}
