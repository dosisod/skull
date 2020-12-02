; ModuleID = './test/sh/oper/mult_float_consts.sk'
source_filename = "./test/sh/oper/mult_float_consts.sk"

@x = private global double 0.000000e+00

define i64 @main() {
entry:
  store double 6.000000e+00, double* @x
  ret i64 0
}
