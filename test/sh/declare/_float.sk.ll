; ModuleID = './test/sh/declare/float.sk'
source_filename = "./test/sh/declare/float.sk"

define i64 @main() {
entry:
  %x = alloca double
  store double 3.141500e+00, double* %x
  ret i64 0
}
