; ModuleID = './test/sh/declare/float_infinity.sk'
source_filename = "./test/sh/declare/float_infinity.sk"

@x = private global double 0.000000e+00

define i64 @main() {
entry:
  store double 0x7FF0000000000000, double* @x
  ret i64 0
}
