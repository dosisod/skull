; ModuleID = './test/sh/function/non_int_return.sk'
source_filename = "./test/sh/function/non_int_return.sk"

define i64 @.non_int_return() {
entry:
  ret i64 0
}

define private double @func() {
entry:
  %ret = alloca double
  store double 3.140000e+00, double* %ret
  %0 = load double, double* %ret
  ret double %0
}

define private double @func2() {
entry:
  ret double 3.140000e+00
}
