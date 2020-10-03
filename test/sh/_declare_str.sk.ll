; ModuleID = './test/sh/declare_str.sk'
source_filename = "./test/sh/declare_str.sk"

define i64 @main() {
entry:
  %x = alloca [7 x i8]
  store [7 x i8] c"abcdef\00", [7 x i8]* %x
}
