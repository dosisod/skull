; ModuleID = './test/sh/declare_mut_explicit.sk'
source_filename = "./test/sh/declare_mut_explicit.sk"

define i64 @main() {
entry:
  %x = alloca i64
  store i64 0, i64* %x
}
