; ModuleID = './test/sh/declare/float_negative.sk'
source_filename = "./test/sh/declare/float_negative.sk"

@x = private global double 0.000000e+00

define i64 @.float_negative() {
entry:
  store double -1.234000e+03, double* @x
  ret i64 0
}
