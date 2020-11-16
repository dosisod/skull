; ModuleID = './test/sh/declare/float_infinity.sk'
source_filename = "./test/sh/declare/float_infinity.sk"

define i64 @main() {
entry:
  %x = alloca double
  store double 0x7FF0000000000000, double* %x
  ret i64 0
}
