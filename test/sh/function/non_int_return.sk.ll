; ModuleID = './test/sh/function/non_int_return.sk'
source_filename = "./test/sh/function/non_int_return.sk"

define i64 @.non_int_return() {
entry:
  ret i64 0
}

define private double @func() {
entry:
  %ret = alloca double, align 8
  store double 3.140000e+00, ptr %ret, align 8
  %0 = load double, ptr %ret, align 8
  ret double %0
}

define private double @func2() {
entry:
  ret double 3.140000e+00
}
