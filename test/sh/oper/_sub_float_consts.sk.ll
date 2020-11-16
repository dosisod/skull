; ModuleID = './test/sh/oper/sub_float_consts.sk'
source_filename = "./test/sh/oper/sub_float_consts.sk"

define i64 @main() {
entry:
  %x = alloca double
  store double -1.000000e+00, double* %x
  ret i64 0
}
