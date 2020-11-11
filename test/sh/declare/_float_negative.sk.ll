; ModuleID = './test/sh/declare/float_negative.sk'
source_filename = "./test/sh/declare/float_negative.sk"

define i64 @main() {
entry:
  %x = alloca double
  store double -1.234000e+03, double* %x
}
