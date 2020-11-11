; ModuleID = './test/sh/oper/div_float_consts.sk'
source_filename = "./test/sh/oper/div_float_consts.sk"

define i64 @main() {
entry:
  %x = alloca double
  store double 2.000000e+00, double* %x
}
