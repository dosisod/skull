; ModuleID = './test/sh/oper/mult_float_consts.sk'
source_filename = "./test/sh/oper/mult_float_consts.sk"

define i64 @main() {
entry:
  %x = alloca double
  store double 6.000000e+00, double* %x
}
