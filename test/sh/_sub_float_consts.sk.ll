; ModuleID = './test/sh/sub_float_consts.sk'
source_filename = "./test/sh/sub_float_consts.sk"

define i64 @main() {
entry:
  %x = alloca double
  store double 1.000000e+00, double* %x
  store double -1.000000e+00, double* %x
}
