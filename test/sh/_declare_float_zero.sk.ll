; ModuleID = './test/sh/declare_float_zero.sk'
source_filename = "./test/sh/declare_float_zero.sk"

define i64 @main() {
entry:
  %x = alloca double
  store double 0.000000e+00, double* %x
}