; ModuleID = './test/sh/oper/add_float_consts.sk'
source_filename = "./test/sh/oper/add_float_consts.sk"

define i64 @main() {
entry:
  %x = alloca double
  store double 5.000000e+00, double* %x
  ret i64 0
}