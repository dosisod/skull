; ModuleID = './test/sh/oper/float_pow.sk'
source_filename = "./test/sh/oper/float_pow.sk"

@x = private global double 0.000000e+00

define i64 @.float_pow() {
entry:
  %0 = call double @.float_pow.1(double 2.000000e+00, double 3.000000e+00)
  store double %0, double* @x
  ret i64 0
}

declare double @.float_pow.1(double, double)
