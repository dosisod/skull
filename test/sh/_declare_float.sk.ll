; ModuleID = './test/sh/declare_float.sk'
source_filename = "./test/sh/declare_float.sk"

define i64 @main() {
entry:
  %x = alloca double
  store double 3.141500e+00, double* %x
}
