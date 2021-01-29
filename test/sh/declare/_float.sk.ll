; ModuleID = './test/sh/declare/float.sk'
source_filename = "./test/sh/declare/float.sk"

@x = private global double 0.000000e+00

define i64 @.float() {
entry:
  store double 3.141500e+00, double* @x
  ret i64 0
}
