; ModuleID = './test/sh/function/non_int_return.sk'
source_filename = "./test/sh/function/non_int_return.sk"

define i64 @main() {
entry:
  ret i64 0
}

define private double @func() {
entry:
  ret double 3.140000e+00
}
